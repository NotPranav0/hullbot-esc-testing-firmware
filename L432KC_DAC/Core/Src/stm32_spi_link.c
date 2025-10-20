#include "stm32_spi_link.h"
#include "meas_config.h"
#include "main.h"

static uint8_t spi_tx_buf[BUFFER_SIZE];
static uint8_t spi_rx_buf[BUFFER_SIZE];

static SPI_HandleTypeDef *h_spi;

static void spi_int_assert(void) {
	printf("assert\r\n");
    HAL_GPIO_WritePin(SPI_INT_GPIO_Port, SPI_INT_Pin, GPIO_PIN_SET);
}

static void spi_int_deassert(void) {
	printf("deassert\r\n");
    HAL_GPIO_WritePin(SPI_INT_GPIO_Port, SPI_INT_Pin, GPIO_PIN_RESET);
}

/**
 * @brief This function contains the application logic to process a fully received packet.
 * @note It should be called AFTER the full packet is received via DMA.
 */
static void process_packet(const uint8_t* buffer, uint16_t total_len) {
	printf("process_packet\r\n");
    Commands_t command = buffer[0];
    uint16_t payload_len = (buffer[1] << 8) | buffer[2];

    // Check if the received length matches the header length
    if (payload_len != total_len - 3) {
        // Handle length mismatch error
        return;
    }

    // Point to the payload, which starts after the 3-byte header
    const uint8_t* payload = &buffer[3];

    switch (command) {
        case SET_CONFIG:
            if (payload_len == sizeof(MeasConfig_t)) {
                MeasConfig_t* received_config = (MeasConfig_t*)payload;
                // Use the configuration data...
            } else {
                // Handle size mismatch error
            }
            break;

        case SEND_TEXT:
            // The payload is already in memory. We just need to print it.
            // Note: Best practice is to print from the main loop, not an ISR context.
            // For now, let's just show it works.
            printf("Received Text: %.*s\r\n", payload_len, (char*)payload);
            break;

        default:
            // Handle unknown command
            break;
    }
}

void link_init(SPI_HandleTypeDef *spi) {
    h_spi = spi;
    spi_int_deassert();

    // Start listening for a packet from the Pi.
    if (HAL_SPI_Receive_DMA(h_spi, spi_rx_buf, BUFFER_SIZE) != HAL_OK) {
        Error_Handler();
    }
}

void send_packet_to_pi(uint8_t command, const uint8_t* payload, uint16_t length) {
    if ((length + 3) > BUFFER_SIZE) {
        // Handle error: packet too large
        return;
    }

    // 1. Build the packet header
    spi_tx_buf[0] = command;
    spi_tx_buf[1] = (length >> 8) & 0xFF;
    spi_tx_buf[2] = length & 0xFF;

    // 2. Copy the payload
    memcpy(&spi_tx_buf[3], payload, length);

    // 3. Signal the Pi that data is ready
    spi_int_assert();

    // 4. Start a NON-BLOCKING transfer. The Pi will provide the clock.
    // The HAL_SPI_TxCpltCallback will fire when this is done.
    if (HAL_SPI_Transmit_IT(h_spi, spi_tx_buf, length + 3) != HAL_OK) {
        spi_int_deassert();
        Error_Handler();
    }
}

// --- STM32 HAL CALLBACKS ---

/**
 * @brief Callback for when a reception from the Pi (Master) is complete.
 * @note This is triggered when the Pi raises the Chip Select (CS) line.
 */
void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *spi) {
	HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);

	uint16_t bytes_received = BUFFER_SIZE - __HAL_DMA_GET_COUNTER(spi->hdmarx);

	if (bytes_received >= 3) {
		process_packet(spi_rx_buf, bytes_received);
	}

	// Re-arm the DMA to be ready for the next packet from the Pi
	HAL_SPI_Receive_DMA(h_spi, spi_rx_buf, BUFFER_SIZE);

}

/**
 * @brief Callback for when a transmission TO the Pi (initiated by us) is complete.
 */
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *spi) {
	// The transfer is done, so we can lower the interrupt line.
	spi_int_deassert();
}

/**
 * @brief  SPI error callback.
 * @param  hspi: SPI handle
 * @note   This function is executed when a communication error is detected.
 */
void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi) {
	uint32_t error_code = HAL_SPI_GetError(hspi);

	printf("--- SPI ERROR! ---\r\n");

	if (error_code & HAL_SPI_ERROR_OVR) {
		printf("Error Type: Overrun (OVR)\r\n");
	}
	if (error_code & HAL_SPI_ERROR_MODF) {
		printf("Error Type: Mode Fault (MODF)\r\n");
	}
	if (error_code & HAL_SPI_ERROR_CRC) {
		printf("Error Type: CRC Error\r\n");
	}

	// IMPORTANT: You MUST clear the error flags and re-start the reception,
	// otherwise the SPI peripheral will be stuck.
	HAL_SPI_Abort(hspi); // Abort current operations
	if (HAL_SPI_Receive_DMA(h_spi, spi_rx_buf, BUFFER_SIZE) != HAL_OK) {
		Error_Handler(); // Failed to re-arm
	}
}

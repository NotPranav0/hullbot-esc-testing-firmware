#include <config.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#include "rpi_spi_link.h"
#include "lcd.h"
#include "main.h"

/*
    Pi -> STM32 Commands
    SET_CONFIG: Update threshold configuration 
    SEND_TEXT:  Pi requests STM32 to process text command
*/
typedef enum {
	SET_CONFIG = 0x01,
	SEND_TEXT = 0x02,
    FLASH_FIRMWARE_STATUS = 0x03
} rx_commands_t;

/*
    PING:       STM32 sends a ping to Pi
    LOG_MESSAGE: STM32 sends a log message to Pi
    DEBUG_INFO: STM32 sends debug information to Pi
    FLASH_FIRMWARE_REQUEST: STM32 sends firmware flash request to Pi
    POWEROFF: STM32 sends poweroff command to Pi
*/
typedef enum {
	PING = 0x80,
	LOG_MESSAGE = 0x81,
	DEBUG_INFO = 0x82,
    FLASH_FIRMWARE_REQUEST = 0x83,
    POWEROFF = 0x84
} tx_commands_t;

static uint8_t spi_tx_buf[BUFFER_SIZE];
static uint8_t spi_rx_buf[BUFFER_SIZE];
static uint8_t rx_copy_buffer[BUFFER_SIZE];

static SPI_HandleTypeDef* h_spi;
volatile bool* h_packet_recieved;

static config_t* h_config;

#define TX_QUEUE_SIZE 10

typedef struct {
    uint8_t data[BUFFER_SIZE];
} tx_packet_t;

static tx_packet_t tx_queue[TX_QUEUE_SIZE];
static volatile uint8_t tx_head = 0;
static volatile uint8_t tx_tail = 0;
static volatile bool tx_active = false;

void load_next_packet(void);

void spi_int_assert(void);
void spi_int_deassert(void);
void start_listening(void);
void process_text(char* text, uint16_t len, uint8_t* ret_buf);
void process_config(config_t* received_config);
void send_packet_to_pi(tx_commands_t command, const uint8_t* payload, uint16_t length);

// Public

/*
    Initializes RPi SPI link
    @param spi: Pointer to initialized SPI_HandleTypeDef
    @param packet_recieved: Pointer to boolean flag set when packet is recieved
    @param config: Pointer to current config_t struct
*/
void link_init(SPI_HandleTypeDef *spi, bool* packet_recieved, config_t* config) {
	h_config = config;
    h_spi = spi;
    h_packet_recieved = packet_recieved;

    HAL_GPIO_WritePin(SHUTDOWN_GPIO_Port, SHUTDOWN_Pin, GPIO_PIN_SET);

    memset(spi_tx_buf, 0, BUFFER_SIZE);
    memset(spi_rx_buf, 0, BUFFER_SIZE);
    memset(rx_copy_buffer, 0, BUFFER_SIZE);

    tx_head = 0;
    tx_tail = 0;
    tx_active = false;

    start_listening();
}

/*
    Processes incoming SPI packet from RPi
    @param ret_buf: Pointer to buffer to store any return text data
    @return uint32_t: Length of return data stored in ret_buf
*/
uint32_t link_process_packet(uint8_t* ret_buf) {
	rx_commands_t command = rx_copy_buffer[0];
	uint16_t payload_len = (rx_copy_buffer[1] << 8) | rx_copy_buffer[2];
    const uint8_t* payload = &rx_copy_buffer[3];

    switch (command) {
        case SET_CONFIG:
        	process_config((config_t*)payload);
        	*h_packet_recieved = false;
            return 0;
            break;
        case SEND_TEXT:
            process_text((char*)payload, payload_len, ret_buf);
            *h_packet_recieved = false;
            return payload_len;
            break;
        case FLASH_FIRMWARE_STATUS:
            process_text((char*)payload, payload_len, ret_buf);
            *h_packet_recieved = false;
            return payload_len;
        default:
        	*h_packet_recieved = false;
            break;
    }

    return 0;
}

/*
    Sends formatted log message to RPi
    @param msg: Format string (like printf)
*/
void rpi_printf(const char* msg, ...) {
		char text[PAYLOAD_SIZE];
		va_list args;
		va_start(args, msg);
		vsnprintf(text, sizeof(text), msg, args);
		va_end(args);
		send_packet_to_pi(LOG_MESSAGE, (const uint8_t *)text, strlen(text));
}

/*
    Sends debug information to RPi
    @param data: Pointer to debug data
    @param length: Length of debug data
*/
void rpi_send_debug_info(const uint8_t* data, uint16_t length) {
    send_packet_to_pi(DEBUG_INFO, data, length);
}

/*
    Sends firmware flash request to RPi
*/
void rpi_send_firmware_flash_request() {
    // arbritrary data
    const char* data = "flash_firmware";
    const uint16_t length = strlen(data);

    send_packet_to_pi(FLASH_FIRMWARE_REQUEST, (const uint8_t*)data, length);
}

/*
    Sends poweroff command to RPi
*/
void rpi_press_power_button(void) {
    const char* data = "poweroff";
    send_packet_to_pi(POWEROFF, (const uint8_t*)data, strlen(data));
}

/*
    Pings the RPi to check if it is awake
    @return bool: true if RPi responded, false otherwise
*/
bool rpi_is_awake(void) {
	const char* text = "ping";
	send_packet_to_pi(PING, (const uint8_t*)text, strlen(text));
	HAL_Delay(10);
	if (*h_packet_recieved) {
		*h_packet_recieved = false;
		return true;
	}
	return false;
}


// Private

/*
    Asserts the SPI interrupt line to notify RPi of incoming data
*/
void spi_int_assert(void) {
    HAL_GPIO_WritePin(SPI_INT_GPIO_Port, SPI_INT_Pin, GPIO_PIN_SET);
}

/*
    Deasserts the SPI interrupt line
*/
void spi_int_deassert(void) {
    HAL_GPIO_WritePin(SPI_INT_GPIO_Port, SPI_INT_Pin, GPIO_PIN_RESET);
}

/*
    Starts listening for incoming SPI data from RPi using DMA (Non-blocki)
*/
void start_listening(void) {
	while (HAL_SPI_TransmitReceive_DMA(h_spi, spi_tx_buf, spi_rx_buf, BUFFER_SIZE) != HAL_OK) {}
}

/*
    Processes text command received from RPi
    @param text: Pointer to received text
    @param len: Length of received text
    @param ret_buf: Pointer to buffer to store any return text data
*/
void process_text(char* text, uint16_t len, uint8_t* ret_buf) {
    if(strncmp(text, "pong", len) == 0) {
    	//do nothing
    } else if(strncmp(text, "reset", len) == 0) {
        NVIC_SystemReset();
    } else {
    	// no associated command, pass text back to caller
    	memcpy(ret_buf, text, len);
    }
}

/*
    Processes received configuration data from RPi
    @param received_config: Pointer to received config_t struct
*/
void process_config(config_t* received_config) {
	config_apply(received_config);
}

/*
    Loads next packet from queue into TX buffer
*/
void load_next_packet(void) {
    // if queue is empty, set tx_active to false and clear TX buffer
    if (tx_head == tx_tail) {
        tx_active = false;
        memset(spi_tx_buf, 0, BUFFER_SIZE);
        return;
    }

    tx_active = true;
    memcpy(spi_tx_buf, tx_queue[tx_head].data, BUFFER_SIZE);
    tx_head = (tx_head + 1) % TX_QUEUE_SIZE;
}

/*
    Sends packet to RPi over SPI
    @param command: Command type to send
    @param payload: Pointer to payload data
    @param length: Length of payload data
*/
void send_packet_to_pi(tx_commands_t command, const uint8_t* payload, uint16_t length) {
    if ((length + 3) > BUFFER_SIZE) {
       Error_Handler();
       return;
    }

    __disable_irq();

    // drop packet if queue is full
    if (((tx_tail + 1) % TX_QUEUE_SIZE) == tx_head) {
    	__enable_irq();
    	return;
    }

    tx_packet_t* p = &tx_queue[tx_tail];
    memset(p->data, 0, BUFFER_SIZE);

    p->data[0] = command;
    p->data[1] = (length >> 8) & 0xFF;
    p->data[2] = length & 0xFF;


    memcpy(&p->data[3], payload, length);

    tx_tail = (tx_tail + 1) % TX_QUEUE_SIZE;

    if (!tx_active) {
    	HAL_SPI_Abort(h_spi);
    	load_next_packet();
    	start_listening();
    	if (tx_active) {
    		spi_int_assert();
    	}
    }

    __enable_irq();
}

// STM32 HAL_SPI CALLBACKS
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *spi) {
	spi_int_deassert();
	*h_packet_recieved = true;
	memcpy(rx_copy_buffer, spi_rx_buf, BUFFER_SIZE);
	load_next_packet();
	start_listening();
	if (tx_active) {
		spi_int_assert();
	}
}

void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi) {
	HAL_SPI_Abort(hspi);
	start_listening();
}

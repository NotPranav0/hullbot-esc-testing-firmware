#include "stm32_spi_link.h"
#include "main.h"

static volatile bool tx_ready = false;
static volatile uint16_t tx_len = 0;
static uint8_t tx_buf[TX_MAX];
static uint8_t dummy_tx[TX_MAX] = {0};

static uint8_t rx_buf[RX_MAX];
static uint8_t dummy_rx[RX_MAX];
static uint16_t rx_len = 0;
static bool rx_has_new_data = false;

static volatile app_state_t g_state = ST_IDLE;

static void spi_int_assert(void)
{
	printf("spi-int-asserted with tx_len = %d", tx_len);
	HAL_GPIO_WritePin(SPI_INT_GPIO_Port, SPI_INT_Pin, GPIO_PIN_SET);
}

static void spi_int_deassert(void)
{
	printf("spi-int-deasserted ");
	HAL_GPIO_WritePin(SPI_INT_GPIO_Port, SPI_INT_Pin, GPIO_PIN_RESET);
}

app_state_t link_get_state(void)
{
	return g_state;
}

const char* link_get_state_str(void)
{
	return app_state_name((app_state_t) g_state);
}

const char* app_state_name(app_state_t state)
{
	switch (state)
	{
	case ST_IDLE:
		return "IDLE";
	case ST_RUN:
		return "RUN";
	case ST_ERROR:
		return "ERROR";
	default:
		return "?";
	}
}

void app_state_init(void)
{
	g_state = ST_IDLE;
}

void link_send_string(const char *string)
{
	if (!string)
	{
		return;
	}

	size_t length = strlen(string);

	if (length > (TX_MAX - 1))
	{
		length = TX_MAX - 1;
	}

	memcpy(tx_buf, string, length);

	tx_len = (uint16_t) length;
	tx_ready = true;

	spi_int_assert();
}

void link_on_timer_tick(void)
{
	HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);

	g_state = (app_state_t) ((g_state + 1) % ST_COUNT);

	link_send_string(app_state_name(g_state));
}

void link_init(void)
{
	spi_int_deassert();
	app_state_init();
}


void recieve_from_pi()
{
	printf("recieving-string-from-pi ");
	HAL_TIM_Base_Stop_IT(h_tim);
	printf("timer-stopped ");
	uint8_t length_bytes[2];
	if (HAL_SPI_TransmitReceive(h_spi, dummy_tx, length_bytes, sizeof(length_bytes), HAL_MAX_DELAY) != HAL_OK)
	{
		printf("not-sure ");
		printf("timer-restarted ");
		HAL_TIM_Base_Start_IT(h_tim);
		return;
	}
	uint16_t length = length_bytes[0] << 8 | length_bytes[1];
	printf("going-to-recieve-%d-bytes ", length);

	if (HAL_SPI_TransmitReceive(h_spi, dummy_tx, rx_buf, length, HAL_MAX_DELAY) == HAL_OK)
	{
		rx_len = length;
		rx_has_new_data = true;
		printf("rx-has-new-data ");
	} else {
		printf("rx-failed ");
	}
	HAL_TIM_Base_Start_IT(h_tim);
	return;
}

void send_header()
{
	printf("sending-header-to-pi ");
	uint8_t header[3];
	if (tx_ready)
	{
		header[0] = STATUS_READY;
		header[1] = (tx_len >> 8) & 0xFF;
		header[2] = tx_len & 0xFF;
		printf("[%d, %d, %d]", header[0], header[1], header[2]);
	}
	else
	{
		header[0] = STATUS_EMPTY;
		header[1] = 0;
		header[2] = 0;
		printf("tx-not-ready ");
	}
	HAL_SPI_TransmitReceive(h_spi, header, dummy_rx, sizeof(header), 10);
	printf("successfully-sent-header ");
	return;
}
void send_payload()
{
	printf("sending-string-to-pi ");

	if (tx_ready)
	{

		HAL_SPI_TransmitReceive(h_spi, tx_buf, dummy_rx, tx_len, 1000);
		printf("transfered-tx-buf=%s", tx_buf);
		__disable_irq();
		tx_ready = false;
		__enable_irq();
	}
	else
	{
		printf("tx-wasnt-ready ");
	}

	spi_int_deassert();
	return;
}

void handle_received_command()
{
	if (strncmp((const char *)rx_buf, "ping", 4) == 0)
	{

		link_send_string("pong");
	}
	else if (strncmp((const char *)rx_buf, "restart", 7) == 0)
	{
		HAL_NVIC_SystemReset();
	}
	rx_has_new_data = false;
}


void link_task(opcode_t opcode)
{
	switch (opcode)
	{
		case SEND_BYTE:
			recieve_from_pi();
			break;
		case POLL_BYTE:
			send_header();
			break;
		case READ_BYTE:
			send_payload();
			break;
		default:
			break;
	}

	if (rx_has_new_data)
	{
		handle_received_command();
	}
	return;
}


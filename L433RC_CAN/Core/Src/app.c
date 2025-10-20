#include <stdio.h>

#include "app.h"
#include "tinymovr.h"


CAN_HandleTypeDef* h_can;
UART_HandleTypeDef* h_uart;

void app_init(CAN_HandleTypeDef* can, UART_HandleTypeDef* uart) {
	h_can = can;
	h_uart = uart;

	// disable printf buffering
	setvbuf(stdout, NULL, _IONBF, 0);

	app_main();
}


void setup()
{
	tm_init(h_can);
	tm_set_gains(POS_GAIN, VEL_GAIN);
	tm_set_vel_inc(VEL_INC);
	tm_set_limits(VEL_LIMIT * TICKS_PER_TURN / 60, CURRENT_LIMIT);
	tm_set_encoder_config((tinymovr_encoder_config_t){.type = 0, .bandwidth = ENCODER_BANDWIDTH});
	tm_set_motor_config((tinymovr_motor_config_t){.flags = 0, .pole_pairs = 7, .calibration_constant = CALIBRATION_CURRENT});
	tm_set_state((tinymovr_state_config_t){.state = CALIBRATE, .mode = 0, .errors = 0});

	//TODO: this is a long time!
	for (int idx = 0; idx < 1000; idx++)
	{
		tinymovr_state_config_t state = tm_get_state();
		if (state.state == IDLE)
		{
			break;
		}
		HAL_Delay(50);
	}
	tinymovr_state_config_t state = tm_get_state();
	if (state.state != IDLE && state.errors != 0)
	{
		printf("esc could not be calibrated\r\n");
	}
	tm_save_config();

}

void spin()
{

	tm_set_state((tinymovr_state_config_t){.state = CONTROL, .mode = VELOCITY, .errors = 0});
	tm_set_vel_setpoint(SPIN_TEST_VEL_SETPOINT);

	float est_vel = tm_get_encoder_estimates();
	if (est_vel + THRESHOLD < SPIN_TEST_VEL_SETPOINT || est_vel - THRESHOLD > SPIN_TEST_VEL_SETPOINT)
	{
		printf("spin test pos dir failed\r\n");
	}

	tm_set_vel_setpoint(-1*SPIN_TEST_VEL_SETPOINT);

	est_vel = tm_get_encoder_estimates();
	if (est_vel + THRESHOLD < -1*SPIN_TEST_VEL_SETPOINT || est_vel - THRESHOLD > -1*SPIN_TEST_VEL_SETPOINT)
	{
		printf("spin test neg dir failed\r\n");
	}

	tm_set_vel_setpoint(0);
	tm_set_state((tinymovr_state_config_t){.state = IDLE, .mode = 0, .errors = 0});

	printf("spin test passed!\r\n");;
}


void app_main() {
	setup();
	spin();

}


int _write(int file, char *ptr, int len)
{
	if (h_uart->gState != HAL_UART_STATE_RESET)
	{
		HAL_UART_Transmit(h_uart, (uint8_t*) ptr, (uint16_t) len, 50);
	}
	return len;
}


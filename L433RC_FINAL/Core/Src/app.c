#include <string.h>

#include "app.h"
#include "adc.h"
#include "button.h"
#include "esc.h"
#include "rpi_spi_link.h"
#include "lcd.h"
#include "config.h"
#include "tinymovr.h"

#include "stm32l4xx_hal.h"

//#define TESTING

static ADC_HandleTypeDef* h_adc;
static CAN_HandleTypeDef* h_can;
static SPI_HandleTypeDef* h_spi;
static I2C_HandleTypeDef* h_i2c;

static config_t* h_config;

void app_main();

bool establish_rpi_connection();
void wait_for_esc_insert();
void resistance_tests();
void voltage_tests();
void flash_esc();
void spin_motor();

void lcd_print_failed_nets(adc_measurement_t* measurements, int num_measurements);
void debug_lcd_print_measurements(adc_measurement_t* measurements);

/*
	Test main replaces app_main for debugging purposes
 */
void test_main() {
	esc_set_pwr(FLOATING);
	esc_set_all_nets_mode(MEASUREMENT);
	adc_set_1v2_source(CONNECTED);

	while (1) {
		adc_measurement_t measurements[NUM_RESISTANCE_CHANNELS] = {0};
		adc_take_measurements(measurements, RESISTANCE);
		config_evaluate_resistances(measurements);
		rpi_send_debug_info((uint8_t*)measurements, sizeof(measurements));
		HAL_Delay(1000);
	}
}

/*
	Initialize application
 */
void app_init(ADC_HandleTypeDef* adc, CAN_HandleTypeDef* can, SPI_HandleTypeDef* spi, I2C_HandleTypeDef* i2c) {
	h_adc = adc;
	h_can = can;
	h_spi = spi;
	h_i2c = i2c;

	h_config = config_init();
	link_init(spi, h_config);
	adc_init(adc);
	lcd_init(i2c);
	tm_init(can);

	esc_set_pwr(FLOATING);
	adc_set_1v2_source(FLOATING);

#ifdef TESTING
	test_main();
#else
	app_main();
#endif

}


void app_main(void) {
	establish_rpi_connection();
	
	while(1) {
		wait_for_esc_insert();

		lcd_printf(LCD_LINE_1, "Press to Start");
		Press_Type_t press_type = PRESS_TYPE_NONE;
		while(press_type == PRESS_TYPE_NONE) {
			press_type = wait_on_button(500);
			
			// esc was removed
			if (!esc_is_connected()) {
				break;
			}
		}

		// go back to waiting for ESC insert
		if (!esc_is_connected()) {
			continue;
		}

		if (press_type == PRESS_TYPE_SHORT) {
			lcd_printf(LCD_LINE_1, "Running Tests");
			resistance_tests();
		} else if (press_type == PRESS_TYPE_LONG) {
			lcd_printf(LCD_LINE_1, "RPI Powered Off");
			rpi_press_power_button();
			HAL_Delay(1500);
		}

	}
}

/*
	Establish connection with RPI, or skip if button pressed
 */
bool establish_rpi_connection() {
	lcd_printf(LCD_LINE_1, "Waiting for RPI");
	lcd_printf(LCD_LINE_2, "Press to Skip");

	Press_Type_t press_type = PRESS_TYPE_NONE;

	while(press_type == PRESS_TYPE_NONE) {
		press_type = wait_on_button(500);
		if (rpi_is_awake()) {
			lcd_printf(LCD_LINE_1, "RPI Connected");
			HAL_Delay(1500);
			return true;
		}
	}

	lcd_printf(LCD_LINE_1, "RPI Connection");
	lcd_printf(LCD_LINE_2, "Skipped");
	HAL_Delay(1500);

	lcd_clear_screen();
	return false;
}

/*
	Wait for ESC to be inserted (blocking)
 */
void wait_for_esc_insert() {
	if (!esc_is_connected()) {
		lcd_printf(LCD_LINE_1, "Insert ESC");

		while (!esc_is_connected()) {

			// handle button press and spi packet processing while waiting for ESC insert
			Press_Type_t press_type = wait_on_button(10);
			if (press_type == PRESS_TYPE_LONG) {
				lcd_printf(LCD_LINE_1, "RPI Powered Off");
				rpi_press_power_button();
				HAL_Delay(1500);
				lcd_printf(LCD_LINE_1, "Insert ESC");
			}

			uint8_t ret_buf[PAYLOAD_SIZE] = {0};
			link_process_packet(ret_buf);
		}
	}
	lcd_clear_screen();
}

/*
	Run resistance tests
	1. Set ESC to measurement mode
	2. Take resistance measurements
	3. Evaluate results
	4. If any failures, display on LCD and wait for ESC removal
	5. If all pass, proceed to voltage tests
 */
void resistance_tests() {
	esc_set_pwr(FLOATING);
	esc_set_all_nets_mode(MEASUREMENT);
	adc_set_1v2_source(CONNECTED);


	adc_measurement_t measurements[NUM_RESISTANCE_CHANNELS] = {0};
	adc_take_measurements(measurements, RESISTANCE);
	bool any_failures = config_evaluate_resistances(measurements);
	rpi_send_debug_info((uint8_t*)measurements, sizeof(measurements));
	if (any_failures) {
		lcd_printf(LCD_LINE_1, "Short Detected");
		while (esc_is_connected()) {
			lcd_print_failed_nets(measurements, NUM_RESISTANCE_CHANNELS);
		}
		return;
	} else {
		voltage_tests();
	}
}

/*
	Run voltage tests
	1. Set ESC to original mode
	2. Take voltage measurements
	3. Evaluate results
	4. If any failures, display on LCD and wait for ESC removal
	5. If all pass, process to flashing
 */
void voltage_tests() {
	adc_set_1v2_source(FLOATING);
	esc_set_all_nets_mode(ORIGINAL);
	// esc_set_pwr(CONNECTED);
	esc_set_pwr(FLOATING); //TODO


	adc_measurement_t measurements[NUM_VOLTAGE_CHANNELS] = {0};
	adc_take_measurements(measurements, VOLTAGE);
	bool any_failures = config_evaluate_voltages(measurements);
	rpi_send_debug_info((uint8_t*)measurements, sizeof(measurements));
	if (any_failures) {
		lcd_printf(LCD_LINE_1, "Voltage Fault");
		while (esc_is_connected()) {
			lcd_print_failed_nets(measurements, NUM_VOLTAGE_CHANNELS);
		}
		return;
	} else {
		flash_esc();
	}

	return;

}

/*
	Flash ESC firmware via RPi
	1. Set ESC to original mode
	2. Send flash request to RPi
	3. Wait for RPi response
	4. If success, move to spinning motor
	5. If failure, display failure message and wait for ESC removal
 */
void flash_esc() {
	adc_set_1v2_source(FLOATING);
	esc_set_all_nets_mode(ORIGINAL);
	// esc_set_pwr(CONNECTED);
	esc_set_pwr(FLOATING); //TODO

	rpi_send_firmware_flash_request();

	while (1) {
		uint8_t ret_buf[PAYLOAD_SIZE] = {0};
		uint32_t ret_len = link_process_packet(ret_buf);
		if (strncmp((char*)ret_buf, "success", ret_len) == 0) {
			spin_motor();
			break;
		} else if (strncmp((char*)ret_buf, "failure", ret_len) == 0) {
			lcd_printf(LCD_LINE_1, "Flash Failed");
			//TODO could try printing error message from RPi to LCD
			while (esc_is_connected()) {
				continue;
			}
			return;
		} else {
			// unknown response, ignore, wait for next packet
			continue;
		}
	}

}

//TODO 
void spin_motor() {
	esc_set_all_nets_mode(ORIGINAL);
	// esc_set_pwr(CONNECTED); 
	esc_set_pwr(FLOATING); //TODO
	
	uint8_t id = 0;
	while(id < 14) {
		set_can_id(id);
		lcd_printf(LCD_LINE_1, "Trying ID:: 0x%02X", id);
		uint8_t can_id = tm_get_can_id();
		if (can_id != 0xFF) {
			lcd_printf(LCD_LINE_1, "All Tests Pass");
			lcd_printf(LCD_LINE_2, "Remove ESC");
			while (esc_is_connected()) {
				continue;
			}
			return;
		}
		id++;
		HAL_Delay(1000);
	}

	lcd_printf(LCD_LINE_1, "CAN Failed");
	while (esc_is_connected()) {
		continue;
	}
	return;
}

/*
	Print names of failed nets to LCD line 2
 */
void lcd_print_failed_nets(adc_measurement_t* measurements, int num_measurements) {
	char failed_nets_buf[64];
	failed_nets_buf[0] = '\0';
	for (int i = 0; i < num_measurements; i++) {
		if (measurements[i].result == FAIL) {
			if (failed_nets_buf[0] != '\0') {
				strcat(failed_nets_buf, ",");
			}
			strcat(failed_nets_buf, measurements[i].name);
		}
	}
	lcd_printf(LCD_LINE_2, "%s", failed_nets_buf);
}

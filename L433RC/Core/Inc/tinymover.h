#ifndef INC_TINYMOVER_H_
#define INC_TINYMOVER_H_

#include "stm32l4xx_hal.h"
#include <stdint.h>


// TinyMover Endpoints
#define TM_SET_GAINS 0x19
#define TM_SET_VEL_INC 0x2B
#define TM_SET_LIMITS 0x0F
#define TM_GET_ENCODER_CONFIG 0x0B
#define TM_SET_ENCODER_CONFIG 0x11
#define TM_SET_MOTOR_CONFIG 0x1F
#define TM_SET_STATE 0x07
#define TM_GET_STATE 0x03
#define TM_SAVE_CONFIG 0x1C
#define TM_SET_VEL_SETPOINT 0x0D
#define TM_GET_ENCODER_ESTIMATES 0x09

// Config constants
#define ENCODER_BANDWIDTH 1500
#define POS_GAIN 20
#define VEL_GAIN 0.0001
#define VEL_INC 500
#define TICKS_PER_TURN 8192
#define VEL_LIMIT 500
#define CURRENT_LIMIT 11
#define CALIBRATION_CURRENT 4

// Spin test constants
#define CAN_ID 1
#define SPIN_TEST_VEL_SETPOINT 100000
#define THRESHOLD 1000

typedef enum 
{
    IDLE = 0,
    CALIBRATE,
    CONTROL
} tinymovr_state_t;

typedef enum 
{
    CURRENT = 0,
    VELOCITY,
    POSITION
} tinymovr_mode_t;

typedef struct 
{
    tinymovr_state_t state;
    tinymovr_mode_t  mode;
    uint8_t 		 errors;
} tinymovr_state_config_t;

typedef struct 
{
    uint8_t type;
    float 	bandwidth;
} tinymovr_encoder_config_t;

typedef struct 
{
    uint8_t flags;
    uint8_t pole_pairs;
    float 	calibration_constant;
} tinymovr_motor_config_t;

typedef struct 
{
    CAN_TxHeaderTypeDef tx_header;
    uint8_t 			tx_data[8];
    uint32_t 			mailbox;
	CAN_HandleTypeDef* 	h_can;
} tinymovr_can_tx_t;

typedef struct 
{
    CAN_RxHeaderTypeDef rx_header;
    uint8_t 			rx_data[8];
    CAN_FilterTypeDef 	canfil;
	CAN_HandleTypeDef* 	h_can;
} tinymovr_can_rx_t;

// set up can
void tm_init(CAN_HandleTypeDef* can);

void tm_set_gains(float position, float velocity); // 0x19
void tm_set_vel_inc(float increment); //0x2B
void tm_set_limits(float velocity, float current); //0x0F
tinymovr_encoder_config_t tm_get_encoder_config(void); //0x0B
void tm_set_encoder_config(tinymovr_encoder_config_t config); //0x11
void tm_set_motor_config(tinymovr_motor_config_t config); //0x1F
void tm_set_state(tinymovr_state_config_t config); //0x07
tinymovr_state_config_t tm_get_state(void); //0x03
void tm_save_config(void); //0x1C
void tm_set_vel_setpoint(float velocity); //0x0D
float tm_get_encoder_estimates(void); //0x09

void float32_to_4_byte_array(float value, uint8_t* array);

#endif /* INC_TINYMOVER_H_ */

/*
	thruster:
	0: # encoder
		tracking_bandwidth: 1500
		pos_gain: 20
		vel_gain: 0.0001
		vel_inc: 500
		ticks_per_turn: 8192
	both:
		limits: [500, 11]  # Speed limit is in RPM, converted to hall or encoder
		Ical: 4



	// set gains
	tm.set_gains(p["pos_gain"], p["vel_gain"])
	// set inc
	tm.set_vel_inc(p["vel_inc"])
	// set limits
	tm.set_limits(b["limits"][0] * p["ticks_per_turn"] / 60, b["limits"][1])
	// get encoder config
	tm.encoder_config["type"]
	// set encoder config
	tm.set_encoder_config(encoder_config.value, p["tracking_bandwidth"])
	// check if same type, if not same type, reboot esc

	// set motor config
	tm.set_motor_config(0, 7, b["Ical"])
	// calibrate
	tmx.set_state(1, 0)
	//check for errors
	tmx.state
	// save config
	tm.save_config()

	*/
    // // spin test
	// tm.velocity_control()
	// tm.set_vel_setpoint(1e5)
	// tmx.encoder_estimates
	// CHECK VEL
	// tm.set_vel_setpoint(-1e5)
	// tmx.encoder_estimates
	// CHECK VEL
	// tm.set_vel_setpoint(0)
	// tm.idle()

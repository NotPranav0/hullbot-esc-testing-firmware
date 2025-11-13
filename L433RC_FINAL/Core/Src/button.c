#include <stdbool.h>

#include "button.h"
#include "stm32l4xx_hal.h"
#include "main.h"

#define DEBOUNCE_TIME   50 
#define LONG_PRESS_TIME 500 // Time in ms to consider a press as long press


/*
    Waits for button press or timeout
    @param timeout: Maximum time to wait for button press (ms)
    @return PRESS_TYPE_SHORT if short press detected
            PRESS_TYPE_LONG if long press detected
            PRESS_TYPE_NONE if timeout expired before press, or if press was a glitch
*/
Press_Type_t wait_on_button(uint32_t timeout) {
    bool long_press_detected = false;

    uint32_t function_start_tick = HAL_GetTick();

    while (HAL_GPIO_ReadPin(START_BUTTON_GPIO_Port, START_BUTTON_Pin) == GPIO_PIN_SET) {
        // Spin and wait until button is pressed (active low)

        // Check if the timeout has expired
        if (HAL_GetTick() - function_start_tick >= timeout) {
            return PRESS_TYPE_NONE;
        }

        HAL_Delay(1);
    }

    // Button is low. Start debounce timer.
    HAL_Delay(DEBOUNCE_TIME);

    // If pin is HIGH, it was a glitch.
    if (HAL_GPIO_ReadPin(START_BUTTON_GPIO_Port, START_BUTTON_Pin) == GPIO_PIN_SET) {
        return PRESS_TYPE_NONE;
    }

    //Press is confirmed. Wait for release
    uint32_t press_start_tick = HAL_GetTick();

    while (HAL_GPIO_ReadPin(START_BUTTON_GPIO_Port, START_BUTTON_Pin) == GPIO_PIN_RESET) {
        // Button is still being held down

        uint32_t press_duration = HAL_GetTick() - press_start_tick;

        if (!long_press_detected && (press_duration >= LONG_PRESS_TIME)) {
            long_press_detected = true;
        }

        HAL_Delay(1);
    }

    HAL_Delay(DEBOUNCE_TIME);

    if (long_press_detected) {
        return PRESS_TYPE_LONG;
    } else {
        return PRESS_TYPE_SHORT;
    }
}


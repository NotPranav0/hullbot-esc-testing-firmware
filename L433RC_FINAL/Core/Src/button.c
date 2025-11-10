#include <stdbool.h>

#include "button.h"
#include "stm32l4xx_hal.h"
#include "main.h"

Press_Type_t WaitFor_Button_Press(void) {
    uint32_t press_start_tick;
    bool long_press_detected = false;


    while (HAL_GPIO_ReadPin(START_BUTTON_GPIO_Port, START_BUTTON_Pin) == GPIO_PIN_SET) {
        // Spin and wait until button is pressed (active low)
    }

    // Button is low. Start debounce timer.
    HAL_Delay(DEBOUNCE_TIME);

    // If pin is HIGH, it was a glitch. Return NONE and try again.
    if (HAL_GPIO_ReadPin(START_BUTTON_GPIO_Port, START_BUTTON_Pin) == GPIO_PIN_SET) {
        return PRESS_TYPE_NONE;
    }

    //Press is confirmed. Wait for release
    press_start_tick = HAL_GetTick();

    while (HAL_GPIO_ReadPin(START_BUTTON_GPIO_Port, START_BUTTON_Pin) == GPIO_PIN_RESET) {
        // Button is still being held down

        uint32_t press_duration = HAL_GetTick() - press_start_tick;

        if (!long_press_detected && (press_duration >= LONG_PRESS_TIME)) {
            long_press_detected = true;
        }

        HAL_Delay(5);
    }

    //Button has been released. Debounce release.
    HAL_Delay(DEBOUNCE_TIME);

    if (long_press_detected) {
        return PRESS_TYPE_LONG;
    } else {
        return PRESS_TYPE_SHORT;
    }
}


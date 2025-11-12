#ifndef INC_BUTTON_H_
#define INC_BUTTON_H_

#include <stdint.h>

typedef enum {
    PRESS_TYPE_NONE,
    PRESS_TYPE_SHORT,
    PRESS_TYPE_LONG
} Press_Type_t;

Press_Type_t wait_on_button(uint32_t timeout);

#endif /* INC_BUTTON_H_ */

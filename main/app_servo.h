#ifndef APP_SERVO_H
#define APP_SERVO_H

#include <stdint.h>

bool servo_init(void);
bool servo_set_angle(uint8_t index, uint8_t angle);
uint8_t servo_get_angle(uint8_t index);

#endif // APP_SERVO_H

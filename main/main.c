#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "app_servo.h"

extern void xiaozhi_app_main(void);

void app_main(void)
{
    printf("Starting WALL-E...\n");
    // app_servo_init();
    xiaozhi_app_main();
}

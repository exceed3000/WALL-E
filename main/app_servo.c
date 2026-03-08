#include "app_servo.h"
#include "esp_log.h"
#include "driver/mcpwm_prelude.h"

static const char *TAG = "servo";

#define SERVO_1_GPIO      CONFIG_SERVO_1_GPIO_PIN
#define SERVO_2_GPIO      CONFIG_SERVO_2_GPIO_PIN
#define SERVO_3_GPIO      CONFIG_SERVO_3_GPIO_PIN
#define SERVO_4_GPIO      CONFIG_SERVO_4_GPIO_PIN
#define SERVO_5_GPIO      CONFIG_SERVO_5_GPIO_PIN
#define SERVO_6_GPIO      CONFIG_SERVO_6_GPIO_PIN
#define SERVO_7_GPIO      CONFIG_SERVO_7_GPIO_PIN

#define MAX_SERVOS        7

static uint8_t s_current_angles[MAX_SERVOS] = {90, 90, 90, 90, 90, 90, 90};
static mcpwm_cmpr_handle_t s_comparators[MAX_SERVOS] = {NULL};

bool servo_init(void)
{
    ESP_LOGI(TAG, "Init %d servos", MAX_SERVOS);
    
    const int servo_gpios[MAX_SERVOS] = {
        SERVO_1_GPIO, SERVO_2_GPIO, SERVO_3_GPIO, SERVO_4_GPIO,
        SERVO_5_GPIO, SERVO_6_GPIO, SERVO_7_GPIO
    };
    
    mcpwm_timer_handle_t timer = NULL;
    mcpwm_timer_config_t timer_config = {
        .group_id = 0,
        .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT,
        .resolution_hz = 1000000,
        .count_mode = MCPWM_TIMER_COUNT_MODE_UP_DOWN,
        .period_ticks = 20000,
    };
    
    if (mcpwm_new_timer(&timer_config, &timer) != ESP_OK) {
        ESP_LOGE(TAG, "Create timer failed");
        return false;
    }
    
    for (int i = 0; i < MAX_SERVOS; i++) {
        mcpwm_comparator_config_t comparator_config = {0};
        if (mcpwm_new_comparator(MCPWM_UNIT_0, &comparator_config, &s_comparators[i]) != ESP_OK) {
            ESP_LOGE(TAG, "Create comparator %d failed", i);
            return false;
        }
        
        mcpwm_gen_handle_t generator = NULL;
        mcpwm_generator_config_t generator_config = {
            .gen_gpio_num = servo_gpios[i],
        };
        
        if (mcpwm_new_generator(MCPWM_UNIT_0, &generator_config, &generator) != ESP_OK) {
            ESP_LOGE(TAG, "Create generator %d failed", i);
            return false;
        }
        
        mcpwm_comparator_connect(timer, s_comparators[i]);
        mcpwm_generator_connect(timer, generator);
        
        mcpwm_comparator_set_compare_value(s_comparators[i], 1500);
        ESP_LOGI(TAG, "Servo %d initialized on GPIO %d", i + 1, servo_gpios[i]);
    }
    
    mcpwm_timer_enable(timer);
    mcpwm_timer_start_stop(timer, MCPWM_TIMER_START_NO_STOP_IN_EN);
    
    ESP_LOGI(TAG, "All servos initialized");
    return true;
}

bool servo_set_angle(uint8_t index, uint8_t angle)
{
    if (index >= MAX_SERVOS) {
        ESP_LOGE(TAG, "Invalid servo index: %d", index);
        return false;
    }
    
    if (angle > 180) {
        ESP_LOGE(TAG, "Invalid angle: %d", angle);
        return false;
    }
    
    uint32_t pulse_width = 500 + (angle * 2000 / 180);
    mcpwm_comparator_set_compare_value(s_comparators[index], pulse_width);
    s_current_angles[index] = angle;
    
    ESP_LOGI(TAG, "Servo %d: %d°", index + 1, angle);
    return true;
}

uint8_t servo_get_angle(uint8_t index)
{
    if (index >= MAX_SERVOS) {
        return 0;
    }
    return s_current_angles[index];
}

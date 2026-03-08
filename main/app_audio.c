#include "app_audio.h"
#include "esp_log.h"
#include "driver/i2s.h"

static const char *TAG = "audio";

#define I2S_AUDIO_OUT_NUM     I2S_NUM_1
#define I2S_AUDIO_IN_NUM      I2S_NUM_0
#define I2S_SAMPLE_RATE       (16000)

#define I2S1_WS_PIN           CONFIG_I2S1_WS_PIN
#define I2S1_SCK_PIN          CONFIG_I2S1_SCK_PIN
#define I2S1_SD_PIN           CONFIG_I2S1_SD_PIN

#define I2S0_SD_PIN           CONFIG_I2S0_SD_PIN
#define I2S0_WS_PIN           CONFIG_I2S0_WS_PIN
#define I2S0_SCK_PIN          CONFIG_I2S0_SCK_PIN

static bool s_initialized = false;
static bool s_recording = false;
static bool s_playing = false;

bool audio_init(const audio_config_t* config)
{
    if (s_initialized) return true;
    
    i2s_config_t i2s_out_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
        .sample_rate = config ? config->sample_rate : I2S_SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_RIGHT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 8,
        .dma_buf_len = 1024,
        .use_apll = true,
        .tx_desc_auto_clear = true,
    };
    
    if (i2s_driver_install(I2S_AUDIO_OUT_NUM, &i2s_out_config, 0, NULL) != ESP_OK) {
        ESP_LOGE(TAG, "I2S1 install failed");
        return false;
    }
    
    i2s_pin_config_t i2s1_pin_config = {
        .bck_io_num = I2S1_SCK_PIN,
        .ws_io_num = I2S1_WS_PIN,
        .data_out_num = I2S1_SD_PIN,
        .data_in_num = I2S_PIN_NO_CHANGE,
    };
    
    if (i2s_set_pin(I2S_AUDIO_OUT_NUM, &i2s1_pin_config) != ESP_OK) {
        ESP_LOGE(TAG, "I2S1 set pin failed");
        return false;
    }
    
    i2s_config_t i2s_in_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
        .sample_rate = config ? config->sample_rate : I2S_SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_RIGHT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 8,
        .dma_buf_len = 1024,
        .use_apll = true,
    };
    
    if (i2s_driver_install(I2S_AUDIO_IN_NUM, &i2s_in_config, 0, NULL) != ESP_OK) {
        ESP_LOGE(TAG, "I2S0 install failed");
        i2s_driver_uninstall(I2S_AUDIO_OUT_NUM);
        return false;
    }
    
    i2s_pin_config_t i2s0_pin_config = {
        .bck_io_num = I2S0_SCK_PIN,
        .ws_io_num = I2S0_WS_PIN,
        .data_out_num = I2S_PIN_NO_CHANGE,
        .data_in_num = I2S0_SD_PIN,
    };
    
    if (i2s_set_pin(I2S_AUDIO_IN_NUM, &i2s0_pin_config) != ESP_OK) {
        ESP_LOGE(TAG, "I2S0 set pin failed");
        i2s_driver_uninstall(I2S_AUDIO_OUT_NUM);
        i2s_driver_uninstall(I2S_AUDIO_IN_NUM);
        return false;
    }
    
    s_initialized = true;
    ESP_LOGI(TAG, "Audio initialized: I2S1 Out (%d,%d,%d), I2S0 In (%d,%d,%d)", 
             I2S1_WS_PIN, I2S1_SCK_PIN, I2S1_SD_PIN,
             I2S0_WS_PIN, I2S0_SCK_PIN, I2S0_SD_PIN);
    return true;
}

bool audio_start_recording(void)
{
    if (!s_initialized) return false;
    s_recording = true;
    return true;
}

void audio_stop_recording(void)
{
    s_recording = false;
}

int audio_read(int16_t* buffer, size_t size)
{
    if (!s_initialized || !s_recording) return 0;
    
    size_t bytes_read;
    if (i2s_read(I2S_AUDIO_IN_NUM, buffer, size, &bytes_read, portMAX_DELAY) != ESP_OK) {
        return 0;
    }
    return bytes_read;
}

bool audio_start_playback(void)
{
    if (!s_initialized) return false;
    s_playing = true;
    return true;
}

void audio_stop_playback(void)
{
    s_playing = false;
}

int audio_write(const int16_t* buffer, size_t size)
{
    if (!s_initialized || !s_playing) return 0;
    
    size_t bytes_written;
    if (i2s_write(I2S_AUDIO_OUT_NUM, buffer, size, &bytes_written, portMAX_DELAY) != ESP_OK) {
        return 0;
    }
    return bytes_written;
}

#include <stdio.h>
#include "wifi.h"
#include "set_data_on_site.h"
#include "driver/gpio.h"
#include "esp_sntp.h"
#include "time.h"
#include "global_values.h"

static const char *TAG = "main";
TaskHandle_t set_data_on_site_handle = NULL;
TaskHandle_t spi_get_meteo_data_handle = NULL;

static struct tm g_timeinfo;

//WORD_ALIGNED_ATTR static char spi_slave_tx_buf[32];
WORD_ALIGNED_ATTR static char spi_slave_rx_buf[32];

// queue to hold measurement data
QueueHandle_t meteo_data_queue;

// SPI Host
spi_host_device_t SPI_HOST_STM = SPI2_HOST;

// SPI mutex
SemaphoreHandle_t spi_mutex;

void init_handshake_pin() {
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << READY_PIN),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);
    gpio_set_level(READY_PIN, 0); // Початковий стан — LOW
}

esp_err_t synh_time(void)
{
    /* Initialize SNTP */
    esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
    esp_sntp_setservername(0, "time.google.com");
    esp_sntp_init();

    // Set timezone to Eastern European Time
    setenv("TZ", "EET-2EEST,M3.5.0/3,M10.5.0/4", 1);
    tzset();

    /* Wait for time to be set */
    int retry = 0;
    const int retry_count = 10;
    while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && ++retry < retry_count) {
        printf("Waiting for time synchronization... (%d/%d)\n", retry, retry_count);
        vTaskDelay(pdMS_TO_TICKS(2000));
    }

    return sntp_get_sync_status() == SNTP_SYNC_STATUS_COMPLETED ? ESP_OK : ESP_FAIL;
}

esp_err_t get_curr_time()
{
    // Get current date and time
    time_t now;
    time(&now);
    localtime_r(&now, &g_timeinfo);

    // Format and print the date and time
    char strftime_buf[64];
    strftime(strftime_buf, sizeof(strftime_buf), "%H:%M:%S%d.%m.%y", &g_timeinfo);
    ESP_LOGI(TAG, "Current time: %s", strftime_buf);

    return ESP_OK;
}

esp_err_t spi_init(void)
{
    // Create SPI mutex
    spi_mutex = xSemaphoreCreateMutex();

    // SPI initialization code here
    spi_bus_config_t buscfg = {
        .mosi_io_num = SPI_MOSI_PIN,
        .miso_io_num = SPI_MISO_PIN,
        .sclk_io_num = SPI_SCK_PIN,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4096,
    };

    // Configuration for the SPI slave interface
    spi_slave_interface_config_t slvcfg = {
        .mode = 1,
        .spics_io_num = SPI_NSS_PIN,
        .flags = 0,
        .queue_size = 10,
    };

    // Initialize SPI bus
    esp_err_t ret = spi_slave_initialize(SPI_HOST_STM, &buscfg, &slvcfg, SPI_DMA_CH_AUTO);

    return ret;
}

void set_curr_time(void)
{
    // Implementation of setting current time
    uint8_t data[6];
    data[0] = (uint8_t)(g_timeinfo).tm_sec;
    data[1] = (uint8_t)(g_timeinfo).tm_min;
    data[2] = (uint8_t)(g_timeinfo).tm_hour;
    data[3] = (uint8_t)(g_timeinfo).tm_mday;
    data[4] = (uint8_t)(g_timeinfo).tm_mon + 1;
    data[5] = (uint8_t)(g_timeinfo).tm_year - 100; // 2026 -> 26

    spi_slave_transaction_t trans_desc = {
        .tx_buffer = data,
        .length = 6 * 8,
        .rx_buffer = spi_slave_rx_buf,
    };

    ESP_LOGI(TAG, "Wait for spi tanssmit: %s", data);

    xSemaphoreTake(spi_mutex, portMAX_DELAY);
    gpio_set_level(READY_PIN, 1); // Indicate ready to receive
    esp_err_t ret = spi_slave_transmit(SPI_HOST_STM, &trans_desc, portMAX_DELAY);
    xSemaphoreGive(spi_mutex);

    printf("SPI transmit result: %d\n", ret);
    gpio_set_level(READY_PIN, 0); // Indicate ready to receive
}

void app_main() 
{
    init_handshake_pin();
    esp_err_t ret;
    // Spi initialization
    ret = spi_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "SPI initialization failed");
        return;
    }
    else ESP_LOGI(TAG, "SPI initialization successful");

    // WiFi initialization
    ret = wifi_init();

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "WiFi initialization failed");
        return;
    }
    else ESP_LOGI(TAG, "WiFi initialization successful");

    vTaskDelay(pdMS_TO_TICKS(5000)); // Delay to allow WiFi to initialize properly

    ret = synh_time();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Time synchronization failed");
        //return;
    }
    else ESP_LOGI(TAG, "Time synchronized successfully");

    vTaskDelay(pdMS_TO_TICKS(3000)); // Delay to allow time synchronization

    // get current time from SNTP
    struct tm timeinfo;
    ret = get_curr_time(&timeinfo);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Getting current time failed");
        return;
    }
    else ESP_LOGI(TAG, "Current time obtained successfully");

    // set current time to stm32f407
    set_curr_time();
    if(ret != ESP_OK) {
        ESP_LOGE(TAG, "Setting current time failed");
        return;
    }
    else ESP_LOGI(TAG, "Current time set successfully");

    vTaskDelay(pdMS_TO_TICKS(2000)); // Delay before starting the task

    // Create queue
    meteo_data_queue = xQueueCreate(2, sizeof(measurement_t));

    xTaskCreate(&spi_get_meteo_data_task, "spi_get_meteo_data_task", 4096, NULL, 5, &spi_get_meteo_data_handle);
    configASSERT(spi_get_meteo_data_handle != NULL);

    // Start task to send data to the site
    xTaskCreate(&set_data_on_site_task, "set_data_on_site_task", 8192, NULL, 5, &set_data_on_site_handle);
    configASSERT(set_data_on_site_handle != NULL);
}
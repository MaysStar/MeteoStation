#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_netif.h"
#include "esp_http_client.h"
#include <stdio.h>
#include "driver/spi_slave.h"
#include "driver/gpio.h"

#define TAG1 "SPI_SLAVE"
#define PIN_MOSI 23
#define PIN_MISO 19
#define PIN_SCLK 18
#define PIN_CS   5
static const char *TAG2 = "http_post";

#define WIFI_SSID "Ukrtelecom_EB36"
#define WIFI_PASS "55011471"
#define SERVER_URL "http://sensordashboard.mypressonline.com/"

TaskHandle_t handle_send_data, handle_get_data_from_stm32;
QueueHandle_t q_measurements;

typedef struct __attribute__((packed)){
    float temperature;
    float humidity;
    float pressure;
} measurement_t;

void spi_slave_init(void)
{
    spi_bus_config_t buscfg = {
        .mosi_io_num = PIN_MOSI,
        .miso_io_num = PIN_MISO,
        .sclk_io_num = PIN_SCLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 256
    };

    spi_slave_interface_config_t slvcfg = {
        .mode = 0,
        .spics_io_num = PIN_CS,
        .queue_size = 3,
        .flags = 0,
    };

    gpio_set_pull_mode(PIN_MOSI, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(PIN_SCLK, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(PIN_CS, GPIO_PULLUP_ONLY);

    ESP_ERROR_CHECK(
        spi_slave_initialize(VSPI_HOST, &buscfg, &slvcfg, SPI_DMA_CH_AUTO)
    );

    ESP_LOGI(TAG1, "SPI slave initialized");
}

void wifi_init(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS
        }
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG2, "Connecting to Wi-Fi...");
    ESP_ERROR_CHECK(esp_wifi_connect());
}

void send_data_task(void *pvParameter)
{
    static char post_data[256];
    measurement_t measurement;

    while (1) {
        // Wait for new measurement data
        xQueueReceive(q_measurements, &measurement, portMAX_DELAY);

        // Get temperature, humidity, pressure from stm32
        float temp = measurement.temperature;
        float pres = measurement.pressure;
        float hum = measurement.humidity;

        snprintf(post_data, sizeof(post_data),
                 "{\"temperature\": %.2f, \"pressure\": %.2f, \"humidity\": %.2f}", temp, pres, hum);

        // config HTTP client
        esp_http_client_config_t config = {
            .url = SERVER_URL,
            .method = HTTP_METHOD_POST,
        };

        // Initialize HTTP client and set headers and post field
        esp_http_client_handle_t client = esp_http_client_init(&config);
        esp_http_client_set_header(client, "Content-Type", "application/json");
        esp_http_client_set_post_field(client, post_data, strlen(post_data));

        // Perform the HTTP POST request
        esp_err_t err = esp_http_client_perform(client);
        if (err == ESP_OK) {
            ESP_LOGI(TAG2, "Data sent: %s", post_data);
        } else {
            ESP_LOGE(TAG2, "HTTP POST failed: %s", esp_err_to_name(err));
        }

        // Cleanup
        esp_http_client_cleanup(client);
    }
}

void get_data_from_stm32(void *pvParameter)
{
    measurement_t measurement;
    uint8_t rx_buf[sizeof(measurement_t)];
    uint8_t tx_buf[sizeof(measurement_t)] = {0};

    spi_slave_transaction_t t = {
        .length = 8 * sizeof(measurement_t),
        .tx_buffer = tx_buf,
        .rx_buffer = rx_buf,
    };

    while (1) {
        ESP_LOGI(TAG1, "Waiting for SPI data...");

        // Block until a transaction is received

        esp_err_t ret = spi_slave_transmit(VSPI_HOST, &t, portMAX_DELAY);

        if (ret == ESP_OK)
        {
            ESP_LOGI(TAG1, "RAW SPI DATA:");
            ESP_LOG_BUFFER_HEX(TAG1, rx_buf, sizeof(measurement_t));
            memcpy(&measurement, rx_buf, sizeof(measurement_t));

            // Send measurement to the queue
            ESP_LOGI(TAG1, "T=%.2f P=%.2f H=%.2f",
                    measurement.temperature,
                    measurement.pressure,
                    measurement.humidity);

            xQueueSend(q_measurements, &measurement, portMAX_DELAY);
        }
    }
}

void app_main(void)
{
    spi_slave_init();
    wifi_init();

    BaseType_t status;
    q_measurements = xQueueCreate(1, sizeof(measurement_t));
    configASSERT( q_measurements != NULL );

    status = xTaskCreate(&send_data_task, "send_data_task", 8192, NULL, 5, &handle_send_data);
    configASSERT( status == pdPASS );

    status = xTaskCreate(&get_data_from_stm32, "get_data_from_stm32", 4096, NULL, 5, &handle_get_data_from_stm32);
    configASSERT( status == pdPASS );
}
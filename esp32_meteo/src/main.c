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
#define SERVER_URL "https://esp32-web-production.up.railway.app/api/data"

TaskHandle_t handle_send_data, handle_get_data_from_stm32;
QueueHandle_t q_measurements;

static const char *root_cert_pem = 
"-----BEGIN CERTIFICATE-----\n"
"MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw\n"
"TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh\n"
"cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4\n"
"WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu\n"
"ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY\n"
"MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc\n"
"h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+\n"
"0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U\n"
"A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW\n"
"T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH\n"
"B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC\n"
"B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv\n"
"KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn\n"
"OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn\n"
"jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw\n"
"qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI\n"
"rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV\n"
"HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq\n"
"hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL\n"
"ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ\n"
"3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK\n"
"NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5\n"
"ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur\n"
"TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC\n"
"jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc\n"
"oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq\n"
"4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA\n"
"mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d\n"
"emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=\n"
"-----END CERTIFICATE-----\n";

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
        esp_http_client_config_t config = {
            .url = SERVER_URL,
            .method = HTTP_METHOD_POST,
            .cert_pem = root_cert_pem,
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

    vTaskDelay(pdMS_TO_TICKS(5000));

    BaseType_t status;
    q_measurements = xQueueCreate(1, sizeof(measurement_t));
    configASSERT( q_measurements != NULL );

    status = xTaskCreate(&send_data_task, "send_data_task", 8192, NULL, 5, &handle_send_data);
    configASSERT( status == pdPASS );

    status = xTaskCreate(&get_data_from_stm32, "get_data_from_stm32", 4096, NULL, 5, &handle_get_data_from_stm32);
    configASSERT( status == pdPASS );
}
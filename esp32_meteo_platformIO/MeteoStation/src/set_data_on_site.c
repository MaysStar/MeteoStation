#include "set_data_on_site.h"
#include "global_values.h"

static const char *TAG = "https_client_task";
static const char *TAG2 = "spi_meteo_data";

// queue to hold measurement data
extern QueueHandle_t meteo_data_queue;

// SPI mutex
extern SemaphoreHandle_t spi_mutex;

void spi_get_meteo_data_task(void* pvParameters)
{
    static measurement_t meteo_data;
    // Implementation to get meteo data from SPI
    uint8_t rx_buf[sizeof(measurement_t)];
    uint8_t tx_buf[sizeof(measurement_t)] = {0};

    spi_slave_transaction_t data = 
    {
        .tx_buffer = tx_buf,
        .rx_buffer = rx_buf,
        .length = 8 * sizeof(measurement_t),
    };

    while(1)
    {
        ESP_LOGI(TAG2, "Waiting for spi data");

        // Get data from stm
        xSemaphoreTake(spi_mutex, portMAX_DELAY);
        esp_err_t ret = spi_slave_transmit(SPI_HOST_STM, &data, portMAX_DELAY);
        xSemaphoreGive(spi_mutex);

        memcpy(&meteo_data, rx_buf, sizeof(measurement_t));

        ESP_LOGI(TAG2, "temp: %.2f, press: %.2f, hum: %.2f", meteo_data.temperature, meteo_data.pressure, meteo_data.humidity);

        xQueueSend(meteo_data_queue, &meteo_data, portMAX_DELAY);

        if(ret != ESP_OK)
        {
            ESP_LOGE(TAG2, "Get data from spi error with code: %d", ret);
        }
        else ESP_LOGI(TAG2, "Get data from spi sucssed with code: %d", ret);
    }
}

void set_data_on_site_task(void* pvParameters) 
{
    // Implementation to get the current time
    ESP_LOGI(TAG, "Sending data to site...");
    // Add code to fetch and log the current time

    static measurement_t meteo_data;
    static char post_data[256];

    while(1)
    {
        xQueueReceive(meteo_data_queue, &meteo_data, portMAX_DELAY);

        memset(post_data, 0, sizeof(post_data));
        snprintf(post_data, sizeof(post_data), "{\"temperature\": %.2f, \"pressure\": %.2f, \"humidity\": %.2f}", meteo_data.temperature, meteo_data.pressure, meteo_data.humidity);

        esp_http_client_config_t https_config = {
            .url = "https://esp32-web-production.up.railway.app/api/data",
            .keep_alive_enable = true,
            .crt_bundle_attach = esp_crt_bundle_attach,
            .event_handler = https_event_handler,
            .method = HTTP_METHOD_POST,
        };

        esp_http_client_handle_t https_handle = esp_http_client_init(&https_config);

        // Set POST data header and POST field
        esp_err_t ret = esp_http_client_set_header(https_handle, "Content-Type", "application/json");
        if(ret != ESP_OK)
        {
            ESP_LOGE(TAG, "Set header error with code: %d", ret);
        }
        else ESP_LOGI(TAG, "Set header sucssed with code: %d", ret);

        ret = esp_http_client_set_post_field(https_handle, post_data, strlen(post_data));
        if(ret != ESP_OK)
        {
            ESP_LOGE(TAG, "Set post field error with code: %d", ret);
        }
        else ESP_LOGI(TAG, "Set post field sucssed with code: %d", ret);

        ret = esp_http_client_perform(https_handle);
        if(ret != ESP_OK)
        {
            ESP_LOGE(TAG, "Client perform error with code: %d", ret);
        }
        else ESP_LOGI(TAG, "Client perform sucssed with code: %d", ret);

        int status_code = esp_http_client_get_status_code(https_handle);
        ESP_LOGI(TAG, "HTTP Status Code: %d", status_code);

        ret = esp_http_client_cleanup(https_handle);

        if(ret != ESP_OK)
        {
            ESP_LOGE(TAG, "Client cleanup error with code: %d", ret);
        }
        else ESP_LOGI(TAG, "Client cleanup sucssed with code: %d", ret);
    }
}

esp_err_t https_event_handler(esp_http_client_event_handle_t event)
{
    ESP_LOGI(TAG, "Client handler start");
    esp_http_client_event_id_t event_id = (esp_http_client_event_id_t)event->event_id;

    switch(event_id)
    {
        case HTTP_EVENT_ERROR:
            ESP_LOGE(TAG, "Client event error");
            break;

        case HTTP_EVENT_ON_DATA:
            ESP_LOGI(TAG, "Client get data");
            break;

        case HTTP_EVENT_ON_FINISH:
            ESP_LOGI(TAG, "Client data receive is finished");
            break;

        default:
            break;
    }

    return ESP_OK;
}

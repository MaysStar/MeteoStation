#include "wifi.h"

static const char *TAG = "wifi_task";

esp_err_t wifi_init(void)
{
    /* Initialize NVS */
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    if(ret != ESP_OK) {
        ESP_LOGE(TAG, "NVS Flash init failed with return code: %d", ret);
        vTaskDelete(NULL);
        return ret;
    }

    /* Initialize the TCP/IP stack */
    ESP_ERROR_CHECK(esp_netif_init());

    /* Create default event loop */
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    /* Initialize WiFi */
    wifi_init_config_t config = WIFI_INIT_CONFIG_DEFAULT();
    ret = esp_wifi_init(&config); 
    if(ret != ESP_OK) {
        ESP_LOGE(TAG, "WiFi init failed with return code: %d", ret);
        vTaskDelete(NULL);
        return ret;
    }
    else ESP_LOGI(TAG, "WiFi initialized with return code: %d", ret);

    /* Create the default WiFi station */
    esp_netif_t *netif = esp_netif_create_default_wifi_sta();
    assert(netif);

    /* Set WiFi mode */
    ret = esp_wifi_set_mode(WIFI_MODE_STA);
    if(ret != ESP_OK) {
        ESP_LOGE(TAG, "Setting WiFi mode failed with return code: %d", ret);
        vTaskDelete(NULL);
        return ret;
    }
    else ESP_LOGI(TAG, "WiFi mode set with return code: %d", ret);

    /* Configure WiFi connection settings */
    wifi_config_t wifi_config = {0};

    /* Configure WiFi connection settings */
    strncpy((char*)wifi_config.sta.ssid, CONFIG_WIFI_SSID, sizeof(wifi_config.sta.ssid) - 1);
    strncpy((char*)wifi_config.sta.password, CONFIG_WIFI_PASSWORD, sizeof(wifi_config.sta.password) - 1);

    wifi_config.sta.ssid[sizeof(wifi_config.sta.ssid) - 1] = '\0';
    wifi_config.sta.password[sizeof(wifi_config.sta.password) - 1] = '\0';

    wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;

    /* Set WiFi configuration */
    ret = esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config);
    if(ret != ESP_OK) {
        ESP_LOGE(TAG, "Setting WiFi config failed with return code: %d", ret);
        vTaskDelete(NULL);
        return ret;
    }
    else ESP_LOGI(TAG, "WiFi config set with return code: %d", ret);    

    /* Set register for event handlers */
    ret = esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, NULL);
    ret = esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, NULL);
    if(ret != ESP_OK) {
        ESP_LOGE(TAG, "Event handler registration failed with return code: %d", ret);
        vTaskDelete(NULL);
        return ret;
    }
    else ESP_LOGI(TAG, "Event handler registered with return code: %d", ret);

    /* Start WiFi */
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_LOGI(TAG, "WiFi started");

    return ret;
}

void wifi_event_handler(void* arg, esp_event_base_t base, int32_t id, void* data)
{
    if(base == WIFI_EVENT)
    {
        switch(id)
        {
            case WIFI_EVENT_STA_START:
                esp_wifi_connect();
                ESP_LOGI(TAG, "WiFi STA Started, connecting to AP...");
                break;
            case WIFI_EVENT_STA_DISCONNECTED:
                ESP_LOGE(TAG, "WiFi STA Disconnected, retrying connection...");
                esp_wifi_connect();
                break;
            default:
                break;
        }
    }
    else if (base == IP_EVENT)
    {
        switch(id)
        {
            case IP_EVENT_STA_GOT_IP:
                ip_event_got_ip_t* event = (ip_event_got_ip_t*) data;
                ESP_LOGI(TAG, "Got IP Address: " IPSTR, IP2STR(&event->ip_info.ip));
                break;
            case IP_EVENT_STA_LOST_IP:
                ESP_LOGE(TAG, "Lost IP Address");
                break;
            default:
                break;
        }
    }
}

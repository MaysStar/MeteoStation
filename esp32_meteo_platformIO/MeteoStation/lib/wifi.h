#ifndef WIFI_H
#define WIFI_H

#include "esp_log.h"
#include "esp_err.h"
#include "esp_wifi.h"
#include "nvs_flash.h"

esp_err_t wifi_init(void);
void wifi_event_handler(void* arg, esp_event_base_t base, int32_t id, void* data);

#endif // WIFI_H
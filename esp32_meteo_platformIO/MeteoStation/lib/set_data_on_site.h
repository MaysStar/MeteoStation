#ifndef SET_DATA_H
#define SET_DATA_H

#include "esp_log.h"
#include "esp_err.h"
#include "esp_http_client.h"
#include "esp_crt_bundle.h"

void set_data_on_site_task(void*);
void spi_get_meteo_data_task(void*);
esp_err_t https_event_handler(esp_http_client_event_handle_t event);

#endif // SET_DATA_H
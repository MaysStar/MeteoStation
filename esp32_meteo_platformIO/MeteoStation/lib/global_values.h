#ifndef GLOBAL_VALUES_H
#define GLOBAL_VALUES_H

#include "esp_log.h"
#include "esp_err.h"
#include "driver/spi_slave.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#define SPI_NSS_PIN 25
#define SPI_SCK_PIN 33
#define SPI_MISO_PIN 32
#define SPI_MOSI_PIN 26
#define READY_PIN 12

// Structure to hold measurement data
typedef struct __attribute__((packed)) {
    float temperature;
    float humidity;
    float pressure;
} measurement_t;

extern spi_host_device_t SPI_HOST_STM;
extern SemaphoreHandle_t spi_mutex;

// queue to hold measurement data
extern QueueHandle_t meteo_data_queue;

#endif // GLOBAL_VALUES_H
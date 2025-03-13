/*
 * SPDX-FileCopyrightText: 2020-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "sht4x.h"
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/i2c_master.h"

static bool init() {
    if (sensirion_i2c_init() != 0)
        return true;
}

extern "C" void app_main(void)
{
    if (init()) {
        printf("[Error] Failed to initialize I2C components.\n");
        return;
    }
    while (sht4x_probe() != STATUS_OK) {
        printf("[Error] SHT sensor probing failed");
        vTaskDelay(1000 / portTICK_PERIOD_MS);/* sleep 1s */
    }
    while (1) {
        printf("SHT sensor probing successful\n");
        int32_t temperature, humidity;
        int8_t ret = sht4x_measure_blocking_read(&temperature, &humidity);

        if (ret == STATUS_OK) {
            printf("measured temperature: %0.2f degreeCelsius, "
                   "measured humidity: %0.2f percentRH\n",
                   temperature / 1000.0f, humidity / 1000.0f);
        } else {
            printf("[Error] error reading SHT40 measurement\n");
        }
        sensirion_sleep_usec(1000000);
    }
}

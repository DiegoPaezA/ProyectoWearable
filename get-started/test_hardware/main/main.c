/**
 * @file main.c
 * @author Diego R. Páez
 * @date 21 Feb 2021
 * @brief File containing code to test the hardware on project wearable board.
 *
 * 
 * @see 
 * @see 
 */
#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "cx_util.h"

/// This is a test string
#define PIN GPIO_NUM_2 ///< onboard Led

/**
 * @brief Task for test onboard Led (pin_2).
 *
 * Detailed explanation.
 */
void blinky(void *params)
{
	gpio_pad_select_gpio(PIN);
	gpio_set_direction(PIN, GPIO_MODE_OUTPUT);
	int isOn = 0;
	while (true)
	{
		isOn = !isOn;
		gpio_set_level(PIN, isOn);
		vTaskDelay(100 / portTICK_PERIOD_MS);
	}
}

/**
 * @brief Task to scan i2c devices connected onboard
 *
 * Detailed explanation.
 */
void i2cloop(void *pvParameter)
{
	while (1)
	{
		i2cScan();
		vTaskDelay(1000 / portTICK_RATE_MS);
	}
}

/**
 * @brief Main Function
 *
 * Detailed explanation.
 */
void app_main(void)
{
	printf("*** Test Hardware - Proyecto Wearable v0.1 ***\n");

	i2cInit();
	uart2Init();
	xTaskCreate(&blinky, "blink led", 2048, NULL, 2, NULL);
	xTaskCreate(&i2cloop, "i2c scan loop", 2048, NULL, 2, NULL);
}

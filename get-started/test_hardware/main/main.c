/// @file main.c
#include <stdio.h>

#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"
#include "esp_err.h"
#include "i2c_scanner.h"

/// This is a test string
#define PIN GPIO_NUM_2 ///< onboard Led

/**
 * Task for test onboard Led (pin_2).
 * 
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
 * Task to scan i2c devices connected onboard
 * 
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
 * main function
 * 
*/
void app_main(void)
{
	printf("Hello world!\n");
	i2cInit();


	xTaskCreate(&blinky, "blink led", 2048, NULL, 2, NULL);
	xTaskCreate(&i2cloop, "i2c scan loop", 2048, NULL, 2, NULL);
}

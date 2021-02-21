/// @file main.c
#include <stdio.h>

#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"
#include "esp_err.h"

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
 * Function to scan i2c devices onboard
 * 
 */
void i2c_scan(void)
{
	int devices_found = 0;
	for (int address = 1; address < 127; address++)
	{
		// create and execute the command link
		i2c_cmd_handle_t cmd = i2c_cmd_link_create();
		i2c_master_start(cmd);
		i2c_master_write_byte(cmd, (address << 1) | I2C_MASTER_WRITE, true);
		i2c_master_stop(cmd);
		if (i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_RATE_MS) == ESP_OK)
		{
			printf("-> found device with address 0x%02x\r\n", address);
			devices_found++;
		}
		i2c_cmd_link_delete(cmd);
	}
	if (devices_found == 0)
	{
		printf("\r\n-> no devices found\r\n");
	}

	printf("\r...scan completed!\r\n");
}
/**
 * Task to scan i2c devices connected onboard
 * 
 */
void i2cloop(void *pvParameter)
{


	while (1)
	{
		i2c_scan();
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

	//configure the i2c controller 0 in master mode, normal speed
	i2c_config_t conf;
	conf.mode = I2C_MODE_MASTER;
	conf.sda_io_num = 21;
	conf.scl_io_num = 22;
	conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
	conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
	conf.master.clk_speed = 100000;
	ESP_ERROR_CHECK(i2c_param_config(I2C_NUM_0, &conf));
	printf("- i2c controller configured\r\n");

	// install the driver
	ESP_ERROR_CHECK(i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0));
	printf("- i2c driver installed\r\n\r\n");

	xTaskCreate(&blinky, "blink led", 2048, NULL, 2, NULL);
	xTaskCreate(&i2cloop, "i2c scan loop", 2048, NULL, 2, NULL);
}

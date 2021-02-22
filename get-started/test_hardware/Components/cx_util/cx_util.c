/**
 * @file cx_util.c
 * @author Diego R. Páez
 * @date 21 Feb 2021
 * @brief File containing utilities to apply communications on main program
 *

 * @see 
 * @see 
 */

#include <stdio.h>
#include <string.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"
#include "esp_err.h"
#include "driver/uart.h"
#include "string.h"

#include <stdio.h>

static const int RX_BUF_SIZE = 1024;

#define TXD_PIN (GPIO_NUM_17)
#define RXD_PIN (GPIO_NUM_16)

/**
 * @brief send data over uart2 serial port
 *
 * Detailed explanation.
 */
int uart2Send(const char *data)
{
	const int len = strlen(data);
	const int txBytes = uart_write_bytes(UART_NUM_2, data, len);

	return txBytes;
}

/**
 * @brief Function to scan i2c devices onboard
 *
 * Detailed explanation.
 */
void i2cScan(void)
{
	int devices_found = 0;
	char data[50];
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

			sprintf(data, "-> found device with address 0x%02x\r\n", address); //mueve el valor de sensorReading a data
			uart2Send(data);
			devices_found++;
		}
		i2c_cmd_link_delete(cmd);
	}
	if (devices_found == 0)
	{
		printf("\r\n-> no devices found\r\n");
		uart2Send("\r\n-> no devices found\r\n");
	}

	printf("\r...scan completed!\r\n");
	uart2Send("\r...scan completed!\r\n");
}
/**
 * @brief Function to configure i2c port
 *
 * Detailed explanation.
 */
void i2cInit(void)
{
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
}

/**
 * @brief Function to configure uart2 port
 *
 * Detailed explanation.
 */
void uart2Init(void)
{
	const uart_config_t uart_config = {
		.baud_rate = 115200,
		.data_bits = UART_DATA_8_BITS,
		.parity = UART_PARITY_DISABLE,
		.stop_bits = UART_STOP_BITS_1,
		.flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
	};
	// We won't use a buffer for sending data.
	uart_driver_install(UART_NUM_2, RX_BUF_SIZE * 2, 0, 0, NULL, 0);
	uart_param_config(UART_NUM_2, &uart_config);
	uart_set_pin(UART_NUM_2, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
}
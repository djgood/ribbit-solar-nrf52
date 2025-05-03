/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

 #include <stdio.h>
 #include <zephyr/kernel.h>
 #include <zephyr/sys/printk.h>
 
 #include <zephyr/usb/usb_device.h>
 #include <zephyr/drivers/uart.h>
 #include <zephyr/drivers/gpio.h>
   
 BUILD_ASSERT(DT_NODE_HAS_COMPAT(DT_CHOSEN(zephyr_console), zephyr_cdc_acm_uart),
              "Console device is not ACM CDC UART device");
 
#define DEV_CONSOLE DEVICE_DT_GET(DT_CHOSEN(zephyr_console))
#define DEV_OTHER   DEVICE_DT_GET(DT_CHOSEN(uart_nrf91_ns))	

const struct device *uart = DEV_OTHER;
const struct device *console = DEV_CONSOLE;

static uint8_t rx_buf[128] = {0};

static void uart_cb(const struct device *dev, struct uart_event *evt, void *ctx)
{
	switch (evt->type) {
	case UART_RX_RDY:
		printk("%c", evt->data.rx.buf[evt->data.rx.offset]);
		break;
	case UART_RX_DISABLED:
		uart_rx_enable(dev, rx_buf, sizeof(rx_buf), 100);
		break;

	default:
		break;
	}
}

 int main(void)
 {
	if (usb_enable(NULL)) {
		return 0;
	}

	printk("Hello World! %s\n", CONFIG_ARCH);

	// int err = uart_callback_set(uart, uart_cb, NULL);
	// if (err) {
	// 	printk("ERROR when setting callback %d\n", err);
	// 	return err;
	// }

	// uart_rx_enable(uart, rx_buf, sizeof(rx_buf), 100);
	unsigned char c;
	int ret;
	while (1) {
		ret = uart_poll_in(uart, &c);
		if (ret < -1) {
			printk("uart errored out\n");
			break;
		} else if (ret == 0) {
			if (c == '\n') {
				printk("");
			} else {
				printk("%c", c);
			}
		}

		ret = uart_poll_in(console, &c);
		if (ret < -1) {
			printk("uart errored out\n");
			break;
		} else if (ret == 0) {
			if (c == '\n') {
				const unsigned char space = 0x0a;
				uart_poll_out(uart, space);
			} else {
				uart_poll_out(uart, c);
			}
		}		
	}

	return 0;
 }

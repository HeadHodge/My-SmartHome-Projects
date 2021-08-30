/* main.c - Application main entry point */

/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <sys/printk.h>
#include <sys/ring_buffer.h>
#include "gatt.h"
#include "uart.h"

#define RING_BUF_SIZE 1024

/* size of stack area used by each thread */
#define STACKSIZE 1024

/* scheduling priority used by each thread */
#define GATT_PRIORITY 0
#define UART_PRIORITY 7

char logBuffer[128];
uint8_t ring_buffer[RING_BUF_SIZE];
struct ring_buf ringbuf;

void printLog(char *msg)
{
	printk("%s\n", msg);
}

void main(void)
{
	ring_buf_init(&ringbuf, sizeof(ring_buffer), ring_buffer);
	printk("start HoG Combo\n");
	console("start HoG Combo");
}

K_THREAD_DEFINE(uartStartId, STACKSIZE, uartStart, NULL, NULL, NULL, UART_PRIORITY, 0, 0);
K_THREAD_DEFINE(gattStartId, STACKSIZE, gattStart, NULL, NULL, NULL, GATT_PRIORITY, 0, 0);

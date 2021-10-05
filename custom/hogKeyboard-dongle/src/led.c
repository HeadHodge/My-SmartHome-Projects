/*
 * Copyright (c) 2019 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <device.h>
#include <drivers/gpio.h>
#include <sys/printk.h>
#include <sys/__assert.h>
#include <string.h>

/* size of stack area used by each thread */
#define STACKSIZE 1024

/* scheduling priority used by each thread */
#define PRIORITY 7

#define LED0_NODE DT_ALIAS(led0)
#define LED1_NODE DT_ALIAS(led1)

#if !DT_NODE_HAS_STATUS(LED0_NODE, okay)
#error "Unsupported board: led0 devicetree alias is not defined"
#endif

#if !DT_NODE_HAS_STATUS(LED1_NODE, okay)
#error "Unsupported board: led1 devicetree alias is not defined"
#endif

int connectState=0, advertiseState=0;

struct led {
	struct gpio_dt_spec spec;
	const char *gpio_pin_name;
};

static const struct led led0 = {
	.spec = GPIO_DT_SPEC_GET_OR(LED0_NODE, gpios, {0}),
	.gpio_pin_name = DT_PROP_OR(LED0_NODE, label, ""),
};

static const struct led led1 = {
	.spec = GPIO_DT_SPEC_GET_OR(LED1_NODE, gpios, {0}),
	.gpio_pin_name = DT_PROP_OR(LED1_NODE, label, ""),
};

void displayLed(const struct led *led, int *state, uint32_t sleep_ms)
{
	const struct gpio_dt_spec *spec = &led->spec;
	int toggle=state[0];
	int ret;

	if (!device_is_ready(spec->port)) {
		printk("Error: %s device is not ready\n", spec->port->name);
		return;
	}

	ret = gpio_pin_configure_dt(spec, GPIO_OUTPUT);
	if (ret != 0) {
		printk("Error %d: failed to configure pin %d (LED '%s')\n",
			ret, spec->pin, led->gpio_pin_name);
		return;
	}

	while (1) {
		if(state[0]==0 || state[0]==1)
			toggle = state[0];
		else
			toggle = !toggle;
		
		gpio_pin_set(spec->port, spec->pin, toggle);

		k_msleep(sleep_ms);
	}
}

void setConnectState(int state)
{
	connectState = state;
}

void setAdvertiseState(int state)
{
	advertiseState = state;
}

void displayConnectState(void)
{
	displayLed(&led0, &connectState, 1000);
}

void displayAdvertiseState(void)
{
	displayLed(&led1, &advertiseState, 100);
}

K_THREAD_DEFINE(displayConnectState_id, STACKSIZE, displayConnectState, NULL, NULL, NULL, PRIORITY, 0, 0);
K_THREAD_DEFINE(displayAdvertiseState_id, STACKSIZE, displayAdvertiseState, NULL, NULL, NULL, PRIORITY, 0, 0);

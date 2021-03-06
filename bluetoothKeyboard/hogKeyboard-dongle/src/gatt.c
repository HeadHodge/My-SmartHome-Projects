/* main.c - Application main entry point */

/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/types.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/printk.h>
#include <sys/byteorder.h>
#include <zephyr.h>

#include <settings/settings.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>

#include "hog.h"
#include "usb.h"
#include "led.h"

int isConnected = 0;

static const struct bt_data ad[] = {
	BT_DATA_BYTES(BT_DATA_GAP_APPEARANCE, 0xc1, 0x03),
	BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
	BT_DATA_BYTES(BT_DATA_UUID16_ALL, BT_UUID_16_ENCODE(BT_UUID_HIDS_VAL), BT_UUID_16_ENCODE(BT_UUID_BAS_VAL)),
};

static void bt_ready(int err)
{
	if (err) {
		printk("Bluetooth init failed (err %d)\n", err);
		return;
	}

	//printk("Bluetooth initialized\n");
	log("Bluetooth initialized");

	//hog_init();

	if(IS_ENABLED(CONFIG_SETTINGS)) settings_load();

	err = bt_le_adv_start(BT_LE_ADV_CONN_NAME, ad, ARRAY_SIZE(ad), NULL, 0);
	
	if (err) {
		//printk("Advertising failed to start (err %d)\n", err);
		log("Advertising failed to start");
		return;
	}

	setConnectState(3);
	setAdvertiseState(3);
	log("Advertising successfully started");
}

static void connected(struct bt_conn *conn, uint8_t err)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	if (err) {
		log("Failed to connect");
		return;
	}

	setConnectState(1);
	setAdvertiseState(1);
	log("Connected!");
	isConnected = 1;
	
	if (bt_conn_set_security(conn, BT_SECURITY_L2)) {
		//printk("Failed to set security\n");
		log("Failed to set security on connection");
	}
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	//printk("Disconnected from %s (reason 0x%02x)\n", addr, reason);
	setConnectState(3);
	setAdvertiseState(3);
	log("Disconnected");
	isConnected = 0;
}

static void security_changed(struct bt_conn *conn, bt_security_t level, enum bt_security_err err)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	if (!err) {
		sprintf(logBuffer, "Security changed: L%d", level);
		log(logBuffer);
	} else {
		sprintf(logBuffer, "Security change failed, level: %d, err: %d", level, err);
		log(logBuffer);
	}
}

static struct bt_conn_cb conn_callbacks = {
	.connected = connected,
	.disconnected = disconnected,
	.security_changed = security_changed,
};

static void auth_pairing_confirm(struct bt_conn *conn)
{
	int reply;
	
	reply = bt_conn_auth_pairing_confirm(conn);
	
	sprintf(logBuffer, "Confirm Pairing, result: %d", reply);
	log(logBuffer);
}

static void auth_cancel(struct bt_conn *conn)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	//printk("Pairing cancelled: %s\n", addr);
	log("Pairing cancel");
}

static struct bt_conn_auth_cb auth_cb_display = {
	.pairing_confirm = auth_pairing_confirm,
	.cancel = auth_cancel,
};

void gattStart(void)
{
	int err;

	err = bt_enable(bt_ready);
	if (err) {
		log("Bluetooth init failed");
		return;
	}
	
	bt_conn_cb_register(&conn_callbacks);
	bt_conn_auth_cb_register(&auth_cb_display);
}

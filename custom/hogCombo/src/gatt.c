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
#include "main.h"
#include "uart.h"

int isConnected = 0;
int isAdvertising = 0;

static const struct bt_data ad[] = {
	BT_DATA_BYTES(BT_DATA_GAP_APPEARANCE, 0xc1, 0x03),
	BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_GAP_NO_TIMEOUT | BT_LE_AD_NO_BREDR)),
	BT_DATA_BYTES(BT_DATA_UUID16_ALL, BT_UUID_16_ENCODE(BT_UUID_HIDS_VAL), BT_UUID_16_ENCODE(BT_UUID_BAS_VAL)),
};

void startAdvertising()
{
	int err=0;
	
	if(isAdvertising != 0) {
		console("Abort startAdvertising: Already advertising");
		return;
	}
	
	err = bt_le_adv_start(BT_LE_ADV_CONN_NAME, ad, ARRAY_SIZE(ad), NULL, 0);
	
	if(err) {
		sprintf(logBuffer, "Abort startAdvertising, err: %d", err);
		console(logBuffer);
		return;
	}
	
	printk("Advertising started\n");
	console("Advertising started");
	isAdvertising = 1;
	//setAdvertiseState(3); //flash advertise led
}

static void bt_ready(int err)
{
	if (err) {
		console("Bluetooth init failed");
		return;
	}

	if(IS_ENABLED(CONFIG_SETTINGS)) settings_load();

	/*
	err = bt_le_adv_start(BT_LE_ADV_CONN_NAME, ad, ARRAY_SIZE(ad), NULL, 0);
	
	if (err) {
		//printk("Advertising failed to start (err %d)\n", err);
		console("Advertising failed to start");
		return;
	}
	*/

	printk("Bluetooth started\n");
	console("Bluetooth started");
	
	isConnected = 0;
	//setConnectState(3); //flash connect led
	
	isAdvertising = 0;
	//setAdvertiseState(1); //solid advertise led
	
	startAdvertising();
}

static void connected(struct bt_conn *conn, uint8_t status)
{
	//char addr[BT_ADDR_LE_STR_LEN];

	//bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
	
	sprintf(logBuffer, "Connected status: %d", status);
	console(logBuffer);

	if(status) {
		console("Failed to connect");
		return;
	}
	
	//if (bt_conn_set_security(conn, BT_SECURITY_L2)) {
	//	console("Failed to set security on connection");
	//}

	isConnected = 1;
	//setConnectState(1); //solid connect led
	
	isAdvertising = 0;
	//setAdvertiseState(1); //solid advertise led
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	//printk("Disconnected from %s (reason 0x%02x)\n", addr, reason);
	sprintf(logBuffer, "Disconnected reason: %d", reason);
	console(logBuffer);
	
	isConnected = 0;
	//setConnectState(3); //flash connect led
	
	isAdvertising = 0;
	//setAdvertiseState(1); //solid advertise led
}

static void security_changed(struct bt_conn *conn, bt_security_t level, enum bt_security_err err)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	if (!err) {
		sprintf(logBuffer, "Security changed: L%d", level);
		console(logBuffer);
	} else {
		sprintf(logBuffer, "Security change failed, level: %d, err: %d", level, err);
		console(logBuffer);
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
	console(logBuffer);
}

static void auth_cancel(struct bt_conn *conn)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	//printk("Pairing cancelled: %s\n", addr);
	console("Pairing cancel");
}

static struct bt_conn_auth_cb auth_cb_display = {
	.pairing_confirm = auth_pairing_confirm,
	.cancel = auth_cancel,
};

void gattStart()
{
	printk("gattStart\n");
	console("gattStart");
	int err;

	err = bt_enable(bt_ready);
	if (err) {
		console("Bluetooth init failed");
		return;
	}
	
	bt_conn_cb_register(&conn_callbacks);
	bt_conn_auth_cb_register(&auth_cb_display);
}

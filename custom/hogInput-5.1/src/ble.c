#include <stdio.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>
#include <settings/settings.h>

struct bt_conn *peerConn = NULL;

static const struct bt_data ad[] = {
	BT_DATA_BYTES(BT_DATA_GAP_APPEARANCE, 0xc1, 0x03),
	BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_GAP_NO_TIMEOUT | BT_LE_AD_NO_BREDR)),
	BT_DATA_BYTES(BT_DATA_UUID16_ALL, BT_UUID_16_ENCODE(BT_UUID_HIDS_VAL), BT_UUID_16_ENCODE(BT_UUID_BAS_VAL)),
};

static void connected(struct bt_conn *conn, uint8_t err)
{
	if (err) {
		printf("Connection failed (err 0x%02x)\n", err);
		peerConn = NULL;
	} else {
		printf("Connected\n");
		peerConn = conn;
	}
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
	printf("Disconnected (reason 0x%02x)\n", reason);
	peerConn = NULL;
}

static void security_changed(struct bt_conn *conn, bt_security_t level, enum bt_security_err err)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	if (!err) {
		printf("Security Changed: L%d\n", level);
	} else {
		printf("Security Change Failed, level: %d, err: %d\n", level, err);
	}
}

BT_CONN_CB_DEFINE(conn_callbacks) = {
	.connected = connected,
	.security_changed = security_changed,
	.disconnected = disconnected,
};

static void passkey_confirm(struct bt_conn *conn, unsigned int passkey)
{
	printf("Passkey Confirm\n");
	bt_conn_auth_passkey_confirm(conn);
}

static void pairing_confirm(struct bt_conn *conn)
{
	printf("Pairing Confirm\n");
	bt_conn_auth_pairing_confirm(conn);
}

static void pairing_complete(struct bt_conn *conn, bool bonded)
{
	printf("Pairing Complete\n");
}

static void pairing_failed(struct bt_conn *conn, enum bt_security_err reason)
{
	printf("Pairing Failed (%d). Disconnecting.\n", reason);
	bt_conn_disconnect(conn, BT_HCI_ERR_AUTH_FAIL);
}

static void auth_cancel(struct bt_conn *conn)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	printf("Pairing Cancelled: %s\n", addr);
}

static struct bt_conn_auth_cb auth_cb_display = {
	.passkey_confirm = passkey_confirm,
	.pairing_confirm = pairing_confirm,
	.pairing_complete = pairing_complete,
	.pairing_failed = pairing_failed,
	.cancel = auth_cancel,
};

int bleStart()
{
	int err;

	err = bt_enable(NULL);
	if (err) {
		printf("Bluetooth init failed (err %d)\n", err);
		return -1;
	}

	printf("Bluetooth initialized\n");
	
	bt_conn_auth_cb_register(&auth_cb_display);
	
	k_sleep(K_SECONDS(5));
	
	//Load previous saved state		
	if (IS_ENABLED(CONFIG_BT_SETTINGS)) settings_load();

	err = bt_le_adv_start(BT_LE_ADV_CONN_NAME, ad, ARRAY_SIZE(ad), NULL, 0);
	if (err) {
		printf("Advertising failed to start (err %d)\n", err);
		return -2;
	}

	printf("Advertising started\n");
	return 0;
}
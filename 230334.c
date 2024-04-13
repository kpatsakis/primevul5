static void conn_set_key(struct hci_conn *conn, u8 key_type, u8 pin_len)
{
	if (key_type == HCI_LK_CHANGED_COMBINATION)
		return;

	conn->pin_length = pin_len;
	conn->key_type = key_type;

	switch (key_type) {
	case HCI_LK_LOCAL_UNIT:
	case HCI_LK_REMOTE_UNIT:
	case HCI_LK_DEBUG_COMBINATION:
		return;
	case HCI_LK_COMBINATION:
		if (pin_len == 16)
			conn->pending_sec_level = BT_SECURITY_HIGH;
		else
			conn->pending_sec_level = BT_SECURITY_MEDIUM;
		break;
	case HCI_LK_UNAUTH_COMBINATION_P192:
	case HCI_LK_UNAUTH_COMBINATION_P256:
		conn->pending_sec_level = BT_SECURITY_MEDIUM;
		break;
	case HCI_LK_AUTH_COMBINATION_P192:
		conn->pending_sec_level = BT_SECURITY_HIGH;
		break;
	case HCI_LK_AUTH_COMBINATION_P256:
		conn->pending_sec_level = BT_SECURITY_FIPS;
		break;
	}
}
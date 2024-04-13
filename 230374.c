static void hci_link_key_request_evt(struct hci_dev *hdev, struct sk_buff *skb)
{
	struct hci_ev_link_key_req *ev = (void *) skb->data;
	struct hci_cp_link_key_reply cp;
	struct hci_conn *conn;
	struct link_key *key;

	BT_DBG("%s", hdev->name);

	if (!hci_dev_test_flag(hdev, HCI_MGMT))
		return;

	hci_dev_lock(hdev);

	key = hci_find_link_key(hdev, &ev->bdaddr);
	if (!key) {
		BT_DBG("%s link key not found for %pMR", hdev->name,
		       &ev->bdaddr);
		goto not_found;
	}

	BT_DBG("%s found key type %u for %pMR", hdev->name, key->type,
	       &ev->bdaddr);

	conn = hci_conn_hash_lookup_ba(hdev, ACL_LINK, &ev->bdaddr);
	if (conn) {
		clear_bit(HCI_CONN_NEW_LINK_KEY, &conn->flags);

		if ((key->type == HCI_LK_UNAUTH_COMBINATION_P192 ||
		     key->type == HCI_LK_UNAUTH_COMBINATION_P256) &&
		    conn->auth_type != 0xff && (conn->auth_type & 0x01)) {
			BT_DBG("%s ignoring unauthenticated key", hdev->name);
			goto not_found;
		}

		if (key->type == HCI_LK_COMBINATION && key->pin_len < 16 &&
		    (conn->pending_sec_level == BT_SECURITY_HIGH ||
		     conn->pending_sec_level == BT_SECURITY_FIPS)) {
			BT_DBG("%s ignoring key unauthenticated for high security",
			       hdev->name);
			goto not_found;
		}

		conn_set_key(conn, key->type, key->pin_len);
	}

	bacpy(&cp.bdaddr, &ev->bdaddr);
	memcpy(cp.link_key, key->val, HCI_LINK_KEY_SIZE);

	hci_send_cmd(hdev, HCI_OP_LINK_KEY_REPLY, sizeof(cp), &cp);

	hci_dev_unlock(hdev);

	return;

not_found:
	hci_send_cmd(hdev, HCI_OP_LINK_KEY_NEG_REPLY, 6, &ev->bdaddr);
	hci_dev_unlock(hdev);
}
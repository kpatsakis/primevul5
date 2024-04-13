static void hci_disconn_complete_evt(struct hci_dev *hdev, struct sk_buff *skb)
{
	struct hci_ev_disconn_complete *ev = (void *) skb->data;
	u8 reason;
	struct hci_conn_params *params;
	struct hci_conn *conn;
	bool mgmt_connected;
	u8 type;

	BT_DBG("%s status 0x%2.2x", hdev->name, ev->status);

	hci_dev_lock(hdev);

	conn = hci_conn_hash_lookup_handle(hdev, __le16_to_cpu(ev->handle));
	if (!conn)
		goto unlock;

	if (ev->status) {
		mgmt_disconnect_failed(hdev, &conn->dst, conn->type,
				       conn->dst_type, ev->status);
		goto unlock;
	}

	conn->state = BT_CLOSED;

	mgmt_connected = test_and_clear_bit(HCI_CONN_MGMT_CONNECTED, &conn->flags);

	if (test_bit(HCI_CONN_AUTH_FAILURE, &conn->flags))
		reason = MGMT_DEV_DISCONN_AUTH_FAILURE;
	else
		reason = hci_to_mgmt_reason(ev->reason);

	mgmt_device_disconnected(hdev, &conn->dst, conn->type, conn->dst_type,
				reason, mgmt_connected);

	if (conn->type == ACL_LINK) {
		if (test_bit(HCI_CONN_FLUSH_KEY, &conn->flags))
			hci_remove_link_key(hdev, &conn->dst);

		hci_req_update_scan(hdev);
	}

	params = hci_conn_params_lookup(hdev, &conn->dst, conn->dst_type);
	if (params) {
		switch (params->auto_connect) {
		case HCI_AUTO_CONN_LINK_LOSS:
			if (ev->reason != HCI_ERROR_CONNECTION_TIMEOUT)
				break;
			/* Fall through */

		case HCI_AUTO_CONN_DIRECT:
		case HCI_AUTO_CONN_ALWAYS:
			list_del_init(&params->action);
			list_add(&params->action, &hdev->pend_le_conns);
			hci_update_background_scan(hdev);
			break;

		default:
			break;
		}
	}

	type = conn->type;

	hci_disconn_cfm(conn, ev->reason);
	hci_conn_del(conn);

	/* The suspend notifier is waiting for all devices to disconnect so
	 * clear the bit from pending tasks and inform the wait queue.
	 */
	if (list_empty(&hdev->conn_hash.list) &&
	    test_and_clear_bit(SUSPEND_DISCONNECTING, hdev->suspend_tasks)) {
		wake_up(&hdev->suspend_wait_q);
	}

	/* Re-enable advertising if necessary, since it might
	 * have been disabled by the connection. From the
	 * HCI_LE_Set_Advertise_Enable command description in
	 * the core specification (v4.0):
	 * "The Controller shall continue advertising until the Host
	 * issues an LE_Set_Advertise_Enable command with
	 * Advertising_Enable set to 0x00 (Advertising is disabled)
	 * or until a connection is created or until the Advertising
	 * is timed out due to Directed Advertising."
	 */
	if (type == LE_LINK)
		hci_req_reenable_advertising(hdev);

unlock:
	hci_dev_unlock(hdev);
}
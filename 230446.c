static void hci_user_confirm_request_evt(struct hci_dev *hdev,
					 struct sk_buff *skb)
{
	struct hci_ev_user_confirm_req *ev = (void *) skb->data;
	int loc_mitm, rem_mitm, confirm_hint = 0;
	struct hci_conn *conn;

	BT_DBG("%s", hdev->name);

	hci_dev_lock(hdev);

	if (!hci_dev_test_flag(hdev, HCI_MGMT))
		goto unlock;

	conn = hci_conn_hash_lookup_ba(hdev, ACL_LINK, &ev->bdaddr);
	if (!conn)
		goto unlock;

	loc_mitm = (conn->auth_type & 0x01);
	rem_mitm = (conn->remote_auth & 0x01);

	/* If we require MITM but the remote device can't provide that
	 * (it has NoInputNoOutput) then reject the confirmation
	 * request. We check the security level here since it doesn't
	 * necessarily match conn->auth_type.
	 */
	if (conn->pending_sec_level > BT_SECURITY_MEDIUM &&
	    conn->remote_cap == HCI_IO_NO_INPUT_OUTPUT) {
		BT_DBG("Rejecting request: remote device can't provide MITM");
		hci_send_cmd(hdev, HCI_OP_USER_CONFIRM_NEG_REPLY,
			     sizeof(ev->bdaddr), &ev->bdaddr);
		goto unlock;
	}

	/* If no side requires MITM protection; auto-accept */
	if ((!loc_mitm || conn->remote_cap == HCI_IO_NO_INPUT_OUTPUT) &&
	    (!rem_mitm || conn->io_capability == HCI_IO_NO_INPUT_OUTPUT)) {

		/* If we're not the initiators request authorization to
		 * proceed from user space (mgmt_user_confirm with
		 * confirm_hint set to 1). The exception is if neither
		 * side had MITM or if the local IO capability is
		 * NoInputNoOutput, in which case we do auto-accept
		 */
		if (!test_bit(HCI_CONN_AUTH_PEND, &conn->flags) &&
		    conn->io_capability != HCI_IO_NO_INPUT_OUTPUT &&
		    (loc_mitm || rem_mitm)) {
			BT_DBG("Confirming auto-accept as acceptor");
			confirm_hint = 1;
			goto confirm;
		}

		/* If there already exists link key in local host, leave the
		 * decision to user space since the remote device could be
		 * legitimate or malicious.
		 */
		if (hci_find_link_key(hdev, &ev->bdaddr)) {
			bt_dev_dbg(hdev, "Local host already has link key");
			confirm_hint = 1;
			goto confirm;
		}

		BT_DBG("Auto-accept of user confirmation with %ums delay",
		       hdev->auto_accept_delay);

		if (hdev->auto_accept_delay > 0) {
			int delay = msecs_to_jiffies(hdev->auto_accept_delay);
			queue_delayed_work(conn->hdev->workqueue,
					   &conn->auto_accept_work, delay);
			goto unlock;
		}

		hci_send_cmd(hdev, HCI_OP_USER_CONFIRM_REPLY,
			     sizeof(ev->bdaddr), &ev->bdaddr);
		goto unlock;
	}

confirm:
	mgmt_user_confirm_request(hdev, &ev->bdaddr, ACL_LINK, 0,
				  le32_to_cpu(ev->passkey), confirm_hint);

unlock:
	hci_dev_unlock(hdev);
}
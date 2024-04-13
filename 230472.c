static void hci_io_capa_request_evt(struct hci_dev *hdev, struct sk_buff *skb)
{
	struct hci_ev_io_capa_request *ev = (void *) skb->data;
	struct hci_conn *conn;

	BT_DBG("%s", hdev->name);

	hci_dev_lock(hdev);

	conn = hci_conn_hash_lookup_ba(hdev, ACL_LINK, &ev->bdaddr);
	if (!conn)
		goto unlock;

	hci_conn_hold(conn);

	if (!hci_dev_test_flag(hdev, HCI_MGMT))
		goto unlock;

	/* Allow pairing if we're pairable, the initiators of the
	 * pairing or if the remote is not requesting bonding.
	 */
	if (hci_dev_test_flag(hdev, HCI_BONDABLE) ||
	    test_bit(HCI_CONN_AUTH_INITIATOR, &conn->flags) ||
	    (conn->remote_auth & ~0x01) == HCI_AT_NO_BONDING) {
		struct hci_cp_io_capability_reply cp;

		bacpy(&cp.bdaddr, &ev->bdaddr);
		/* Change the IO capability from KeyboardDisplay
		 * to DisplayYesNo as it is not supported by BT spec. */
		cp.capability = (conn->io_capability == 0x04) ?
				HCI_IO_DISPLAY_YESNO : conn->io_capability;

		/* If we are initiators, there is no remote information yet */
		if (conn->remote_auth == 0xff) {
			/* Request MITM protection if our IO caps allow it
			 * except for the no-bonding case.
			 */
			if (conn->io_capability != HCI_IO_NO_INPUT_OUTPUT &&
			    conn->auth_type != HCI_AT_NO_BONDING)
				conn->auth_type |= 0x01;
		} else {
			conn->auth_type = hci_get_auth_req(conn);
		}

		/* If we're not bondable, force one of the non-bondable
		 * authentication requirement values.
		 */
		if (!hci_dev_test_flag(hdev, HCI_BONDABLE))
			conn->auth_type &= HCI_AT_NO_BONDING_MITM;

		cp.authentication = conn->auth_type;
		cp.oob_data = bredr_oob_data_present(conn);

		hci_send_cmd(hdev, HCI_OP_IO_CAPABILITY_REPLY,
			     sizeof(cp), &cp);
	} else {
		struct hci_cp_io_capability_neg_reply cp;

		bacpy(&cp.bdaddr, &ev->bdaddr);
		cp.reason = HCI_ERROR_PAIRING_NOT_ALLOWED;

		hci_send_cmd(hdev, HCI_OP_IO_CAPABILITY_NEG_REPLY,
			     sizeof(cp), &cp);
	}

unlock:
	hci_dev_unlock(hdev);
}
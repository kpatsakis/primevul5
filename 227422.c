void __hci_abort_conn(struct hci_request *req, struct hci_conn *conn,
		      u8 reason)
{
	switch (conn->state) {
	case BT_CONNECTED:
	case BT_CONFIG:
		if (conn->type == AMP_LINK) {
			struct hci_cp_disconn_phy_link cp;

			cp.phy_handle = HCI_PHY_HANDLE(conn->handle);
			cp.reason = reason;
			hci_req_add(req, HCI_OP_DISCONN_PHY_LINK, sizeof(cp),
				    &cp);
		} else {
			struct hci_cp_disconnect dc;

			dc.handle = cpu_to_le16(conn->handle);
			dc.reason = reason;
			hci_req_add(req, HCI_OP_DISCONNECT, sizeof(dc), &dc);
		}

		conn->state = BT_DISCONN;

		break;
	case BT_CONNECT:
		if (conn->type == LE_LINK) {
			if (test_bit(HCI_CONN_SCANNING, &conn->flags))
				break;
			hci_req_add(req, HCI_OP_LE_CREATE_CONN_CANCEL,
				    0, NULL);
		} else if (conn->type == ACL_LINK) {
			if (req->hdev->hci_ver < BLUETOOTH_VER_1_2)
				break;
			hci_req_add(req, HCI_OP_CREATE_CONN_CANCEL,
				    6, &conn->dst);
		}
		break;
	case BT_CONNECT2:
		if (conn->type == ACL_LINK) {
			struct hci_cp_reject_conn_req rej;

			bacpy(&rej.bdaddr, &conn->dst);
			rej.reason = reason;

			hci_req_add(req, HCI_OP_REJECT_CONN_REQ,
				    sizeof(rej), &rej);
		} else if (conn->type == SCO_LINK || conn->type == ESCO_LINK) {
			struct hci_cp_reject_sync_conn_req rej;

			bacpy(&rej.bdaddr, &conn->dst);

			/* SCO rejection has its own limited set of
			 * allowed error values (0x0D-0x0F) which isn't
			 * compatible with most values passed to this
			 * function. To be safe hard-code one of the
			 * values that's suitable for SCO.
			 */
			rej.reason = HCI_ERROR_REJ_LIMITED_RESOURCES;

			hci_req_add(req, HCI_OP_REJECT_SYNC_CONN_REQ,
				    sizeof(rej), &rej);
		}
		break;
	default:
		conn->state = BT_CLOSED;
		break;
	}
}
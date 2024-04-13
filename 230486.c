static void hci_conn_complete_evt(struct hci_dev *hdev, struct sk_buff *skb)
{
	struct hci_ev_conn_complete *ev = (void *) skb->data;
	struct inquiry_entry *ie;
	struct hci_conn *conn;

	BT_DBG("%s", hdev->name);

	hci_dev_lock(hdev);

	conn = hci_conn_hash_lookup_ba(hdev, ev->link_type, &ev->bdaddr);
	if (!conn) {
		/* Connection may not exist if auto-connected. Check the inquiry
		 * cache to see if we've already discovered this bdaddr before.
		 * If found and link is an ACL type, create a connection class
		 * automatically.
		 */
		ie = hci_inquiry_cache_lookup(hdev, &ev->bdaddr);
		if (ie && ev->link_type == ACL_LINK) {
			conn = hci_conn_add(hdev, ev->link_type, &ev->bdaddr,
					    HCI_ROLE_SLAVE);
			if (!conn) {
				bt_dev_err(hdev, "no memory for new conn");
				goto unlock;
			}
		} else {
			if (ev->link_type != SCO_LINK)
				goto unlock;

			conn = hci_conn_hash_lookup_ba(hdev, ESCO_LINK,
						       &ev->bdaddr);
			if (!conn)
				goto unlock;

			conn->type = SCO_LINK;
		}
	}

	if (!ev->status) {
		conn->handle = __le16_to_cpu(ev->handle);

		if (conn->type == ACL_LINK) {
			conn->state = BT_CONFIG;
			hci_conn_hold(conn);

			if (!conn->out && !hci_conn_ssp_enabled(conn) &&
			    !hci_find_link_key(hdev, &ev->bdaddr))
				conn->disc_timeout = HCI_PAIRING_TIMEOUT;
			else
				conn->disc_timeout = HCI_DISCONN_TIMEOUT;
		} else
			conn->state = BT_CONNECTED;

		hci_debugfs_create_conn(conn);
		hci_conn_add_sysfs(conn);

		if (test_bit(HCI_AUTH, &hdev->flags))
			set_bit(HCI_CONN_AUTH, &conn->flags);

		if (test_bit(HCI_ENCRYPT, &hdev->flags))
			set_bit(HCI_CONN_ENCRYPT, &conn->flags);

		/* Get remote features */
		if (conn->type == ACL_LINK) {
			struct hci_cp_read_remote_features cp;
			cp.handle = ev->handle;
			hci_send_cmd(hdev, HCI_OP_READ_REMOTE_FEATURES,
				     sizeof(cp), &cp);

			hci_req_update_scan(hdev);
		}

		/* Set packet type for incoming connection */
		if (!conn->out && hdev->hci_ver < BLUETOOTH_VER_2_0) {
			struct hci_cp_change_conn_ptype cp;
			cp.handle = ev->handle;
			cp.pkt_type = cpu_to_le16(conn->pkt_type);
			hci_send_cmd(hdev, HCI_OP_CHANGE_CONN_PTYPE, sizeof(cp),
				     &cp);
		}
	} else {
		conn->state = BT_CLOSED;
		if (conn->type == ACL_LINK)
			mgmt_connect_failed(hdev, &conn->dst, conn->type,
					    conn->dst_type, ev->status);
	}

	if (conn->type == ACL_LINK)
		hci_sco_setup(conn, ev->status);

	if (ev->status) {
		hci_connect_cfm(conn, ev->status);
		hci_conn_del(conn);
	} else if (ev->link_type == SCO_LINK) {
		switch (conn->setting & SCO_AIRMODE_MASK) {
		case SCO_AIRMODE_CVSD:
			if (hdev->notify)
				hdev->notify(hdev, HCI_NOTIFY_ENABLE_SCO_CVSD);
			break;
		}

		hci_connect_cfm(conn, ev->status);
	}

unlock:
	hci_dev_unlock(hdev);

	hci_conn_check_pending(hdev);
}
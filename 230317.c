static void hci_sync_conn_complete_evt(struct hci_dev *hdev,
				       struct sk_buff *skb)
{
	struct hci_ev_sync_conn_complete *ev = (void *) skb->data;
	struct hci_conn *conn;

	BT_DBG("%s status 0x%2.2x", hdev->name, ev->status);

	hci_dev_lock(hdev);

	conn = hci_conn_hash_lookup_ba(hdev, ev->link_type, &ev->bdaddr);
	if (!conn) {
		if (ev->link_type == ESCO_LINK)
			goto unlock;

		/* When the link type in the event indicates SCO connection
		 * and lookup of the connection object fails, then check
		 * if an eSCO connection object exists.
		 *
		 * The core limits the synchronous connections to either
		 * SCO or eSCO. The eSCO connection is preferred and tried
		 * to be setup first and until successfully established,
		 * the link type will be hinted as eSCO.
		 */
		conn = hci_conn_hash_lookup_ba(hdev, ESCO_LINK, &ev->bdaddr);
		if (!conn)
			goto unlock;
	}

	switch (ev->status) {
	case 0x00:
		conn->handle = __le16_to_cpu(ev->handle);
		conn->state  = BT_CONNECTED;
		conn->type   = ev->link_type;

		hci_debugfs_create_conn(conn);
		hci_conn_add_sysfs(conn);
		break;

	case 0x10:	/* Connection Accept Timeout */
	case 0x0d:	/* Connection Rejected due to Limited Resources */
	case 0x11:	/* Unsupported Feature or Parameter Value */
	case 0x1c:	/* SCO interval rejected */
	case 0x1a:	/* Unsupported Remote Feature */
	case 0x1e:	/* Invalid LMP Parameters */
	case 0x1f:	/* Unspecified error */
	case 0x20:	/* Unsupported LMP Parameter value */
		if (conn->out) {
			conn->pkt_type = (hdev->esco_type & SCO_ESCO_MASK) |
					(hdev->esco_type & EDR_ESCO_MASK);
			if (hci_setup_sync(conn, conn->link->handle))
				goto unlock;
		}
		/* fall through */

	default:
		conn->state = BT_CLOSED;
		break;
	}

	bt_dev_dbg(hdev, "SCO connected with air mode: %02x", ev->air_mode);

	switch (conn->setting & SCO_AIRMODE_MASK) {
	case SCO_AIRMODE_CVSD:
		if (hdev->notify)
			hdev->notify(hdev, HCI_NOTIFY_ENABLE_SCO_CVSD);
		break;
	case SCO_AIRMODE_TRANSP:
		if (hdev->notify)
			hdev->notify(hdev, HCI_NOTIFY_ENABLE_SCO_TRANSP);
		break;
	}

	hci_connect_cfm(conn, ev->status);
	if (ev->status)
		hci_conn_del(conn);

unlock:
	hci_dev_unlock(hdev);
}
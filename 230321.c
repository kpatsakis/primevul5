static void hci_le_remote_feat_complete_evt(struct hci_dev *hdev,
					    struct sk_buff *skb)
{
	struct hci_ev_le_remote_feat_complete *ev = (void *)skb->data;
	struct hci_conn *conn;

	BT_DBG("%s status 0x%2.2x", hdev->name, ev->status);

	hci_dev_lock(hdev);

	conn = hci_conn_hash_lookup_handle(hdev, __le16_to_cpu(ev->handle));
	if (conn) {
		if (!ev->status)
			memcpy(conn->features[0], ev->features, 8);

		if (conn->state == BT_CONFIG) {
			__u8 status;

			/* If the local controller supports slave-initiated
			 * features exchange, but the remote controller does
			 * not, then it is possible that the error code 0x1a
			 * for unsupported remote feature gets returned.
			 *
			 * In this specific case, allow the connection to
			 * transition into connected state and mark it as
			 * successful.
			 */
			if ((hdev->le_features[0] & HCI_LE_SLAVE_FEATURES) &&
			    !conn->out && ev->status == 0x1a)
				status = 0x00;
			else
				status = ev->status;

			conn->state = BT_CONNECTED;
			hci_connect_cfm(conn, status);
			hci_conn_drop(conn);
		}
	}

	hci_dev_unlock(hdev);
}
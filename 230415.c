static void hci_cc_read_clock(struct hci_dev *hdev, struct sk_buff *skb)
{
	struct hci_rp_read_clock *rp = (void *) skb->data;
	struct hci_cp_read_clock *cp;
	struct hci_conn *conn;

	BT_DBG("%s", hdev->name);

	if (skb->len < sizeof(*rp))
		return;

	if (rp->status)
		return;

	hci_dev_lock(hdev);

	cp = hci_sent_cmd_data(hdev, HCI_OP_READ_CLOCK);
	if (!cp)
		goto unlock;

	if (cp->which == 0x00) {
		hdev->clock = le32_to_cpu(rp->clock);
		goto unlock;
	}

	conn = hci_conn_hash_lookup_handle(hdev, __le16_to_cpu(rp->handle));
	if (conn) {
		conn->clock = le32_to_cpu(rp->clock);
		conn->clock_accuracy = le16_to_cpu(rp->accuracy);
	}

unlock:
	hci_dev_unlock(hdev);
}
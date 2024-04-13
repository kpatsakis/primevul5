static void hci_cc_read_stored_link_key(struct hci_dev *hdev,
					struct sk_buff *skb)
{
	struct hci_rp_read_stored_link_key *rp = (void *)skb->data;
	struct hci_cp_read_stored_link_key *sent;

	BT_DBG("%s status 0x%2.2x", hdev->name, rp->status);

	sent = hci_sent_cmd_data(hdev, HCI_OP_READ_STORED_LINK_KEY);
	if (!sent)
		return;

	if (!rp->status && sent->read_all == 0x01) {
		hdev->stored_max_keys = rp->max_keys;
		hdev->stored_num_keys = rp->num_keys;
	}
}
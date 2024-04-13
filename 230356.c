static void hci_cc_delete_stored_link_key(struct hci_dev *hdev,
					  struct sk_buff *skb)
{
	struct hci_rp_delete_stored_link_key *rp = (void *)skb->data;

	BT_DBG("%s status 0x%2.2x", hdev->name, rp->status);

	if (rp->status)
		return;

	if (rp->num_keys <= hdev->stored_num_keys)
		hdev->stored_num_keys -= rp->num_keys;
	else
		hdev->stored_num_keys = 0;
}
static void hci_cc_le_read_supported_states(struct hci_dev *hdev,
					    struct sk_buff *skb)
{
	struct hci_rp_le_read_supported_states *rp = (void *) skb->data;

	BT_DBG("%s status 0x%2.2x", hdev->name, rp->status);

	if (rp->status)
		return;

	memcpy(hdev->le_states, rp->le_states, 8);
}
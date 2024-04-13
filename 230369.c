static void hci_cc_le_read_local_features(struct hci_dev *hdev,
					  struct sk_buff *skb)
{
	struct hci_rp_le_read_local_features *rp = (void *) skb->data;

	BT_DBG("%s status 0x%2.2x", hdev->name, rp->status);

	if (rp->status)
		return;

	memcpy(hdev->le_features, rp->features, 8);
}
static void hci_cc_le_read_resolv_list_size(struct hci_dev *hdev,
					   struct sk_buff *skb)
{
	struct hci_rp_le_read_resolv_list_size *rp = (void *) skb->data;

	BT_DBG("%s status 0x%2.2x size %u", hdev->name, rp->status, rp->size);

	if (rp->status)
		return;

	hdev->le_resolv_list_size = rp->size;
}
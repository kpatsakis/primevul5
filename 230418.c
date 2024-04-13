static void hci_cc_read_local_oob_ext_data(struct hci_dev *hdev,
					   struct sk_buff *skb)
{
	struct hci_rp_read_local_oob_ext_data *rp = (void *) skb->data;

	BT_DBG("%s status 0x%2.2x", hdev->name, rp->status);
}
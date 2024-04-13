static void hci_cc_read_local_ext_features(struct hci_dev *hdev,
					   struct sk_buff *skb)
{
	struct hci_rp_read_local_ext_features *rp = (void *) skb->data;

	BT_DBG("%s status 0x%2.2x", hdev->name, rp->status);

	if (rp->status)
		return;

	if (hdev->max_page < rp->max_page)
		hdev->max_page = rp->max_page;

	if (rp->page < HCI_MAX_PAGES)
		memcpy(hdev->features[rp->page], rp->features, 8);
}
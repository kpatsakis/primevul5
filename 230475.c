static void hci_cc_read_num_supported_iac(struct hci_dev *hdev,
					  struct sk_buff *skb)
{
	struct hci_rp_read_num_supported_iac *rp = (void *) skb->data;

	BT_DBG("%s status 0x%2.2x", hdev->name, rp->status);

	if (rp->status)
		return;

	hdev->num_iac = rp->num_iac;

	BT_DBG("%s num iac %d", hdev->name, hdev->num_iac);
}
static void hci_cc_le_read_num_adv_sets(struct hci_dev *hdev,
				      struct sk_buff *skb)
{
	struct hci_rp_le_read_num_supported_adv_sets *rp = (void *) skb->data;

	BT_DBG("%s status 0x%2.2x No of Adv sets %u", hdev->name, rp->status,
	       rp->num_of_sets);

	if (rp->status)
		return;

	hdev->le_num_of_adv_sets = rp->num_of_sets;
}
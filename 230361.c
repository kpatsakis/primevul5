static void hci_cc_read_def_err_data_reporting(struct hci_dev *hdev,
					       struct sk_buff *skb)
{
	struct hci_rp_read_def_err_data_reporting *rp = (void *)skb->data;

	BT_DBG("%s status 0x%2.2x", hdev->name, rp->status);

	if (rp->status)
		return;

	hdev->err_data_reporting = rp->err_data_reporting;
}
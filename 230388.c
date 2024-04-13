static void hci_cc_write_def_err_data_reporting(struct hci_dev *hdev,
						struct sk_buff *skb)
{
	__u8 status = *((__u8 *)skb->data);
	struct hci_cp_write_def_err_data_reporting *cp;

	BT_DBG("%s status 0x%2.2x", hdev->name, status);

	if (status)
		return;

	cp = hci_sent_cmd_data(hdev, HCI_OP_WRITE_DEF_ERR_DATA_REPORTING);
	if (!cp)
		return;

	hdev->err_data_reporting = cp->err_data_reporting;
}
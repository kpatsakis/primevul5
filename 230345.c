static void hci_cc_exit_periodic_inq(struct hci_dev *hdev, struct sk_buff *skb)
{
	__u8 status = *((__u8 *) skb->data);

	BT_DBG("%s status 0x%2.2x", hdev->name, status);

	if (status)
		return;

	hci_dev_clear_flag(hdev, HCI_PERIODIC_INQ);

	hci_conn_check_pending(hdev);
}
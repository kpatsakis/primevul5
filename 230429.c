static void hci_cc_read_bd_addr(struct hci_dev *hdev, struct sk_buff *skb)
{
	struct hci_rp_read_bd_addr *rp = (void *) skb->data;

	BT_DBG("%s status 0x%2.2x", hdev->name, rp->status);

	if (rp->status)
		return;

	if (test_bit(HCI_INIT, &hdev->flags))
		bacpy(&hdev->bdaddr, &rp->bdaddr);

	if (hci_dev_test_flag(hdev, HCI_SETUP))
		bacpy(&hdev->setup_addr, &rp->bdaddr);
}
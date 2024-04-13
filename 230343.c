static void hci_cc_read_local_commands(struct hci_dev *hdev,
				       struct sk_buff *skb)
{
	struct hci_rp_read_local_commands *rp = (void *) skb->data;

	BT_DBG("%s status 0x%2.2x", hdev->name, rp->status);

	if (rp->status)
		return;

	if (hci_dev_test_flag(hdev, HCI_SETUP) ||
	    hci_dev_test_flag(hdev, HCI_CONFIG))
		memcpy(hdev->commands, rp->commands, sizeof(hdev->commands));
}
static void hci_cc_user_passkey_reply(struct hci_dev *hdev, struct sk_buff *skb)
{
	struct hci_rp_user_confirm_reply *rp = (void *) skb->data;

	BT_DBG("%s status 0x%2.2x", hdev->name, rp->status);

	hci_dev_lock(hdev);

	if (hci_dev_test_flag(hdev, HCI_MGMT))
		mgmt_user_passkey_reply_complete(hdev, &rp->bdaddr, ACL_LINK,
						 0, rp->status);

	hci_dev_unlock(hdev);
}
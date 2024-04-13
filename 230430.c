static void hci_user_passkey_request_evt(struct hci_dev *hdev,
					 struct sk_buff *skb)
{
	struct hci_ev_user_passkey_req *ev = (void *) skb->data;

	BT_DBG("%s", hdev->name);

	if (hci_dev_test_flag(hdev, HCI_MGMT))
		mgmt_user_passkey_request(hdev, &ev->bdaddr, ACL_LINK, 0);
}
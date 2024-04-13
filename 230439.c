static void hci_cs_switch_role(struct hci_dev *hdev, u8 status)
{
	struct hci_cp_switch_role *cp;
	struct hci_conn *conn;

	BT_DBG("%s status 0x%2.2x", hdev->name, status);

	if (!status)
		return;

	cp = hci_sent_cmd_data(hdev, HCI_OP_SWITCH_ROLE);
	if (!cp)
		return;

	hci_dev_lock(hdev);

	conn = hci_conn_hash_lookup_ba(hdev, ACL_LINK, &cp->bdaddr);
	if (conn)
		clear_bit(HCI_CONN_RSWITCH_PEND, &conn->flags);

	hci_dev_unlock(hdev);
}
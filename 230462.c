static void hci_reject_conn(struct hci_dev *hdev, bdaddr_t *bdaddr)
{
	struct hci_cp_reject_conn_req cp;

	bacpy(&cp.bdaddr, bdaddr);
	cp.reason = HCI_ERROR_REJ_BAD_ADDR;
	hci_send_cmd(hdev, HCI_OP_REJECT_CONN_REQ, sizeof(cp), &cp);
}
static void send_conn_param_neg_reply(struct hci_dev *hdev, u16 handle,
				      u8 reason)
{
	struct hci_cp_le_conn_param_req_neg_reply cp;

	cp.handle = cpu_to_le16(handle);
	cp.reason = reason;

	hci_send_cmd(hdev, HCI_OP_LE_CONN_PARAM_REQ_NEG_REPLY, sizeof(cp),
		     &cp);
}
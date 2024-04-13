static void abort_conn_complete(struct hci_dev *hdev, u8 status, u16 opcode)
{
	if (status)
		bt_dev_dbg(hdev, "Failed to abort connection: status 0x%2.2x", status);
}
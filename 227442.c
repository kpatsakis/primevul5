static u8 create_default_scan_rsp_data(struct hci_dev *hdev, u8 *ptr)
{
	u8 scan_rsp_len = 0;

	if (hdev->appearance) {
		scan_rsp_len = append_appearance(hdev, ptr, scan_rsp_len);
	}

	return append_local_name(hdev, ptr, scan_rsp_len);
}
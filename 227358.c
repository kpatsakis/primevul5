static void suspend_req_complete(struct hci_dev *hdev, u8 status, u16 opcode)
{
	bt_dev_dbg(hdev, "Request complete opcode=0x%x, status=0x%x", opcode,
		   status);
	if (test_bit(SUSPEND_SCAN_ENABLE, hdev->suspend_tasks) ||
	    test_bit(SUSPEND_SCAN_DISABLE, hdev->suspend_tasks)) {
		clear_bit(SUSPEND_SCAN_ENABLE, hdev->suspend_tasks);
		clear_bit(SUSPEND_SCAN_DISABLE, hdev->suspend_tasks);
		wake_up(&hdev->suspend_wait_q);
	}

	if (test_bit(SUSPEND_SET_ADV_FILTER, hdev->suspend_tasks)) {
		clear_bit(SUSPEND_SET_ADV_FILTER, hdev->suspend_tasks);
		wake_up(&hdev->suspend_wait_q);
	}
}
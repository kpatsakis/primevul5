static void write_iac(struct hci_request *req)
{
	struct hci_dev *hdev = req->hdev;
	struct hci_cp_write_current_iac_lap cp;

	if (!hci_dev_test_flag(hdev, HCI_DISCOVERABLE))
		return;

	if (hci_dev_test_flag(hdev, HCI_LIMITED_DISCOVERABLE)) {
		/* Limited discoverable mode */
		cp.num_iac = min_t(u8, hdev->num_iac, 2);
		cp.iac_lap[0] = 0x00;	/* LIAC */
		cp.iac_lap[1] = 0x8b;
		cp.iac_lap[2] = 0x9e;
		cp.iac_lap[3] = 0x33;	/* GIAC */
		cp.iac_lap[4] = 0x8b;
		cp.iac_lap[5] = 0x9e;
	} else {
		/* General discoverable mode */
		cp.num_iac = 1;
		cp.iac_lap[0] = 0x33;	/* GIAC */
		cp.iac_lap[1] = 0x8b;
		cp.iac_lap[2] = 0x9e;
	}

	hci_req_add(req, HCI_OP_WRITE_CURRENT_IAC_LAP,
		    (cp.num_iac * 3) + 1, &cp);
}
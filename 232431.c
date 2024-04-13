static int mt_reset_resume(struct hid_device *hdev)
{
	mt_release_contacts(hdev);
	mt_set_modes(hdev, HID_LATENCY_NORMAL, true, true);
	return 0;
}
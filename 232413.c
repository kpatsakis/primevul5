static inline int hid_hw_idle(struct hid_device *hdev, int report, int idle,
		int reqtype)
{
	if (hdev->ll_driver->idle)
		return hdev->ll_driver->idle(hdev, report, idle, reqtype);

	return 0;
}
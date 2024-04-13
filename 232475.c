static inline void hid_hw_request(struct hid_device *hdev,
				  struct hid_report *report, int reqtype)
{
	if (hdev->ll_driver->request)
		return hdev->ll_driver->request(hdev, report, reqtype);

	__hid_request(hdev, report, reqtype);
}
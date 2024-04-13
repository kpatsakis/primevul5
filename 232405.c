static inline int hid_hw_output_report(struct hid_device *hdev, __u8 *buf,
					size_t len)
{
	if (len < 1 || len > HID_MAX_BUFFER_SIZE || !buf)
		return -EINVAL;

	if (hdev->ll_driver->output_report)
		return hdev->ll_driver->output_report(hdev, buf, len);

	return -ENOSYS;
}
static inline int hid_hw_raw_request(struct hid_device *hdev,
				  unsigned char reportnum, __u8 *buf,
				  size_t len, unsigned char rtype, int reqtype)
{
	if (len < 1 || len > HID_MAX_BUFFER_SIZE || !buf)
		return -EINVAL;

	return hdev->ll_driver->raw_request(hdev, reportnum, buf, len,
						    rtype, reqtype);
}
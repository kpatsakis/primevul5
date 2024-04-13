static int hidinput_query_battery_capacity(struct hid_device *dev)
{
	u8 *buf;
	int ret;

	buf = kmalloc(4, GFP_KERNEL);
	if (!buf)
		return -ENOMEM;

	ret = hid_hw_raw_request(dev, dev->battery_report_id, buf, 4,
				 dev->battery_report_type, HID_REQ_GET_REPORT);
	if (ret < 2) {
		kfree(buf);
		return -ENODATA;
	}

	ret = hidinput_scale_battery_capacity(dev, buf[1]);
	kfree(buf);
	return ret;
}
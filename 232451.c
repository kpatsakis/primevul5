static void hidinput_led_worker(struct work_struct *work)
{
	struct hid_device *hid = container_of(work, struct hid_device,
					      led_work);
	struct hid_field *field;
	struct hid_report *report;
	int ret;
	u32 len;
	__u8 *buf;

	field = hidinput_get_led_field(hid);
	if (!field)
		return;

	/*
	 * field->report is accessed unlocked regarding HID core. So there might
	 * be another incoming SET-LED request from user-space, which changes
	 * the LED state while we assemble our outgoing buffer. However, this
	 * doesn't matter as hid_output_report() correctly converts it into a
	 * boolean value no matter what information is currently set on the LED
	 * field (even garbage). So the remote device will always get a valid
	 * request.
	 * And in case we send a wrong value, a next led worker is spawned
	 * for every SET-LED request so the following worker will send the
	 * correct value, guaranteed!
	 */

	report = field->report;

	/* use custom SET_REPORT request if possible (asynchronous) */
	if (hid->ll_driver->request)
		return hid->ll_driver->request(hid, report, HID_REQ_SET_REPORT);

	/* fall back to generic raw-output-report */
	len = hid_report_len(report);
	buf = hid_alloc_report_buf(report, GFP_KERNEL);
	if (!buf)
		return;

	hid_output_report(report, buf);
	/* synchronous output report */
	ret = hid_hw_output_report(hid, buf, len);
	if (ret == -ENOSYS)
		hid_hw_raw_request(hid, report->id, buf, len, HID_OUTPUT_REPORT,
				HID_REQ_SET_REPORT);
	kfree(buf);
}
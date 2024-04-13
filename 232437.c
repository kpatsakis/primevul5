static inline void hid_device_io_start(struct hid_device *hid) {
	if (hid->io_started) {
		dev_warn(&hid->dev, "io already started\n");
		return;
	}
	hid->io_started = true;
	up(&hid->driver_input_lock);
}
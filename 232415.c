static inline void hid_device_io_stop(struct hid_device *hid) {
	if (!hid->io_started) {
		dev_warn(&hid->dev, "io already stopped\n");
		return;
	}
	hid->io_started = false;
	down(&hid->driver_input_lock);
}
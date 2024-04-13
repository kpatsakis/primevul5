static int v4l2loopback_lookup(int device_nr,
			       struct v4l2_loopback_device **device)
{
	struct v4l2loopback_lookup_cb_data data = {
		.device_nr = device_nr,
		.device = NULL,
	};
	int err = idr_for_each(&v4l2loopback_index_idr, &v4l2loopback_lookup_cb,
			       &data);
	if (1 == err) {
		if (device)
			*device = data.device;
		return data.device_nr;
	}
	return -ENODEV;
}
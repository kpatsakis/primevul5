static int v4l2loopback_lookup_cb(int id, void *ptr, void *data)
{
	struct v4l2_loopback_device *device = ptr;
	struct v4l2loopback_lookup_cb_data *cbdata = data;
	if (cbdata && device && device->vdev) {
		if (device->vdev->num == cbdata->device_nr) {
			cbdata->device = device;
			cbdata->device_nr = id;
			return 1;
		}
	}
	return 0;
}
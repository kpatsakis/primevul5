static int vidioc_s_ctrl(struct file *file, void *fh, struct v4l2_control *c)
{
	struct v4l2_loopback_device *dev = v4l2loopback_getdevice(file);
	return v4l2loopback_set_ctrl(dev, c->id, c->value);
}
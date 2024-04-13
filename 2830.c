static int vidioc_enum_input(struct file *file, void *fh,
			     struct v4l2_input *inp)
{
	__u32 index = inp->index;
	MARK();

	if (0 != index)
		return -EINVAL;

	/* clear all data (including the reserved fields) */
	memset(inp, 0, sizeof(*inp));

	inp->index = index;
	strlcpy(inp->name, "loopback", sizeof(inp->name));
	inp->type = V4L2_INPUT_TYPE_CAMERA;
	inp->audioset = 0;
	inp->tuner = 0;
	inp->status = 0;

#ifdef V4L2LOOPBACK_WITH_STD
	inp->std = V4L2_STD_ALL;
#ifdef V4L2_IN_CAP_STD
	inp->capabilities |= V4L2_IN_CAP_STD;
#endif
#endif /* V4L2LOOPBACK_WITH_STD */

	return 0;
}
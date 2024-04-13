int v4l_video_std_enumstd(struct v4l2_standard *vs, v4l2_std_id id)
{
	v4l2_std_id curr_id = 0;
	unsigned int index = vs->index, i, j = 0;
	const char *descr = "";

	/* Return -ENODATA if the id for the current input
	   or output is 0, meaning that it doesn't support this API. */
	if (id == 0)
		return -ENODATA;

	/* Return norm array in a canonical way */
	for (i = 0; i <= index && id; i++) {
		/* last std value in the standards array is 0, so this
		   while always ends there since (id & 0) == 0. */
		while ((id & standards[j].std) != standards[j].std)
			j++;
		curr_id = standards[j].std;
		descr = standards[j].descr;
		j++;
		if (curr_id == 0)
			break;
		if (curr_id != V4L2_STD_PAL &&
				curr_id != V4L2_STD_SECAM &&
				curr_id != V4L2_STD_NTSC)
			id &= ~curr_id;
	}
	if (i <= index)
		return -EINVAL;

	v4l2_video_std_construct(vs, curr_id, descr);
	return 0;
}
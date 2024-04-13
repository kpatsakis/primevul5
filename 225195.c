static unsigned int video_translate_cmd(unsigned int cmd)
{
	switch (cmd) {
#ifdef CONFIG_COMPAT_32BIT_TIME
	case VIDIOC_DQEVENT_TIME32:
		return VIDIOC_DQEVENT;
	case VIDIOC_QUERYBUF_TIME32:
		return VIDIOC_QUERYBUF;
	case VIDIOC_QBUF_TIME32:
		return VIDIOC_QBUF;
	case VIDIOC_DQBUF_TIME32:
		return VIDIOC_DQBUF;
	case VIDIOC_PREPARE_BUF_TIME32:
		return VIDIOC_PREPARE_BUF;
#endif
	}
	if (in_compat_syscall())
		return v4l2_compat_translate_cmd(cmd);

	return cmd;
}
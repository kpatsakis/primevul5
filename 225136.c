static int video_get_user(void __user *arg, void *parg,
			  unsigned int real_cmd, unsigned int cmd,
			  bool *always_copy)
{
	unsigned int n = _IOC_SIZE(real_cmd);
	int err = 0;

	if (!(_IOC_DIR(cmd) & _IOC_WRITE)) {
		/* read-only ioctl */
		memset(parg, 0, n);
		return 0;
	}

	/*
	 * In some cases, only a few fields are used as input,
	 * i.e. when the app sets "index" and then the driver
	 * fills in the rest of the structure for the thing
	 * with that index.  We only need to copy up the first
	 * non-input field.
	 */
	if (v4l2_is_known_ioctl(real_cmd)) {
		u32 flags = v4l2_ioctls[_IOC_NR(real_cmd)].flags;

		if (flags & INFO_FL_CLEAR_MASK)
			n = (flags & INFO_FL_CLEAR_MASK) >> 16;
		*always_copy = flags & INFO_FL_ALWAYS_COPY;
	}

	if (cmd == real_cmd) {
		if (copy_from_user(parg, (void __user *)arg, n))
			err = -EFAULT;
	} else if (in_compat_syscall()) {
		err = v4l2_compat_get_user(arg, parg, cmd);
	} else {
		switch (cmd) {
#ifdef CONFIG_COMPAT_32BIT_TIME
		case VIDIOC_QUERYBUF_TIME32:
		case VIDIOC_QBUF_TIME32:
		case VIDIOC_DQBUF_TIME32:
		case VIDIOC_PREPARE_BUF_TIME32: {
			struct v4l2_buffer_time32 vb32;
			struct v4l2_buffer *vb = parg;

			if (copy_from_user(&vb32, arg, sizeof(vb32)))
				return -EFAULT;

			*vb = (struct v4l2_buffer) {
				.index		= vb32.index,
					.type		= vb32.type,
					.bytesused	= vb32.bytesused,
					.flags		= vb32.flags,
					.field		= vb32.field,
					.timestamp.tv_sec	= vb32.timestamp.tv_sec,
					.timestamp.tv_usec	= vb32.timestamp.tv_usec,
					.timecode	= vb32.timecode,
					.sequence	= vb32.sequence,
					.memory		= vb32.memory,
					.m.userptr	= vb32.m.userptr,
					.length		= vb32.length,
					.request_fd	= vb32.request_fd,
			};
			break;
		}
#endif
		}
	}

	/* zero out anything we don't copy from userspace */
	if (!err && n < _IOC_SIZE(real_cmd))
		memset((u8 *)parg + n, 0, _IOC_SIZE(real_cmd) - n);
	return err;
}
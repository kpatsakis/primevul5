static int video_put_user(void __user *arg, void *parg,
			  unsigned int real_cmd, unsigned int cmd)
{
	if (!(_IOC_DIR(cmd) & _IOC_READ))
		return 0;

	if (cmd == real_cmd) {
		/*  Copy results into user buffer  */
		if (copy_to_user(arg, parg, _IOC_SIZE(cmd)))
			return -EFAULT;
		return 0;
	}

	if (in_compat_syscall())
		return v4l2_compat_put_user(arg, parg, cmd);

	switch (cmd) {
#ifdef CONFIG_COMPAT_32BIT_TIME
	case VIDIOC_DQEVENT_TIME32: {
		struct v4l2_event *ev = parg;
		struct v4l2_event_time32 ev32;

		memset(&ev32, 0, sizeof(ev32));

		ev32.type	= ev->type;
		ev32.pending	= ev->pending;
		ev32.sequence	= ev->sequence;
		ev32.timestamp.tv_sec	= ev->timestamp.tv_sec;
		ev32.timestamp.tv_nsec	= ev->timestamp.tv_nsec;
		ev32.id		= ev->id;

		memcpy(&ev32.u, &ev->u, sizeof(ev->u));
		memcpy(&ev32.reserved, &ev->reserved, sizeof(ev->reserved));

		if (copy_to_user(arg, &ev32, sizeof(ev32)))
			return -EFAULT;
		break;
	}
	case VIDIOC_QUERYBUF_TIME32:
	case VIDIOC_QBUF_TIME32:
	case VIDIOC_DQBUF_TIME32:
	case VIDIOC_PREPARE_BUF_TIME32: {
		struct v4l2_buffer *vb = parg;
		struct v4l2_buffer_time32 vb32;

		memset(&vb32, 0, sizeof(vb32));

		vb32.index	= vb->index;
		vb32.type	= vb->type;
		vb32.bytesused	= vb->bytesused;
		vb32.flags	= vb->flags;
		vb32.field	= vb->field;
		vb32.timestamp.tv_sec	= vb->timestamp.tv_sec;
		vb32.timestamp.tv_usec	= vb->timestamp.tv_usec;
		vb32.timecode	= vb->timecode;
		vb32.sequence	= vb->sequence;
		vb32.memory	= vb->memory;
		vb32.m.userptr	= vb->m.userptr;
		vb32.length	= vb->length;
		vb32.request_fd	= vb->request_fd;

		if (copy_to_user(arg, &vb32, sizeof(vb32)))
			return -EFAULT;
		break;
	}
#endif
	}

	return 0;
}
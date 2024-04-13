video_usercopy(struct file *file, unsigned int cmd, unsigned long arg,
		v4l2_kioctl func)
{
	char	sbuf[128];
	void    *mbuf = NULL;
	void	*parg = NULL;
	long	err  = -EINVAL;
	int     is_ext_ctrl;
	size_t  ctrls_size = 0;
	void __user *user_ptr = NULL;
	is_ext_ctrl = (cmd == VIDIOC_S_EXT_CTRLS || cmd == VIDIOC_G_EXT_CTRLS ||
		       cmd == VIDIOC_TRY_EXT_CTRLS);
	/*  Copy arguments into temp kernel buffer  */
	switch (_IOC_DIR(cmd)) {
	case _IOC_NONE:
		parg = NULL;
		break;
	case _IOC_READ:
	case _IOC_WRITE:
	case (_IOC_WRITE | _IOC_READ):
		if (_IOC_SIZE(cmd) <= sizeof(sbuf)) {
			parg = sbuf;
		} else {
			/* too big to allocate from stack */
			mbuf = kmalloc(_IOC_SIZE(cmd), GFP_KERNEL);
			if (NULL == mbuf)
				return -ENOMEM;
			parg = mbuf;
		}
		err = -EFAULT;
		if (_IOC_DIR(cmd) & _IOC_WRITE)
			if (copy_from_user(parg, (void __user *)arg, _IOC_SIZE(cmd)))
				goto out;
		break;
	}
	if (is_ext_ctrl) {
		struct v4l2_ext_controls *p = parg;
		/* In case of an error, tell the caller that it wasn't
		   a specific control that caused it. */
		p->error_idx = p->count;
		user_ptr = (void __user *)p->controls;
		if (p->count) {
			ctrls_size = sizeof(struct v4l2_ext_control) * p->count;
			/* Note: v4l2_ext_controls fits in sbuf[] so mbuf is still NULL. */
			mbuf = kmalloc(ctrls_size, GFP_KERNEL);
			err = -ENOMEM;
			if (NULL == mbuf)
				goto out_ext_ctrl;
			err = -EFAULT;
			if (copy_from_user(mbuf, user_ptr, ctrls_size))
				goto out_ext_ctrl;
			p->controls = mbuf;
		}
	}
	/* call driver */
	err = func(file, cmd, parg);
	if (err == -ENOIOCTLCMD)
		err = -EINVAL;
	if (is_ext_ctrl) {
		struct v4l2_ext_controls *p = parg;
		p->controls = (void *)user_ptr;
		if (p->count && err == 0 && copy_to_user(user_ptr, mbuf, ctrls_size))
			err = -EFAULT;
		goto out_ext_ctrl;
	}
	if (err < 0)
		goto out;
out_ext_ctrl:
	/*  Copy results into user buffer  */
	switch (_IOC_DIR(cmd)) {
	case _IOC_READ:
	case (_IOC_WRITE | _IOC_READ):
		if (copy_to_user((void __user *)arg, parg, _IOC_SIZE(cmd)))
			err = -EFAULT;
		break;
	}
out:
	kfree(mbuf);
	return err;
}

do_fontx_ioctl(int cmd, struct consolefontdesc __user *user_cfd, int perm, struct console_font_op *op)
{
	struct consolefontdesc cfdarg;
	int i;

	if (copy_from_user(&cfdarg, user_cfd, sizeof(struct consolefontdesc))) 
		return -EFAULT;
 	
	switch (cmd) {
	case PIO_FONTX:
		if (!perm)
			return -EPERM;
		op->op = KD_FONT_OP_SET;
		op->flags = KD_FONT_FLAG_OLD;
		op->width = 8;
		op->height = cfdarg.charheight;
		op->charcount = cfdarg.charcount;
		op->data = cfdarg.chardata;
		return con_font_op(vc_cons[fg_console].d, op);
	case GIO_FONTX: {
		op->op = KD_FONT_OP_GET;
		op->flags = KD_FONT_FLAG_OLD;
		op->width = 8;
		op->height = cfdarg.charheight;
		op->charcount = cfdarg.charcount;
		op->data = cfdarg.chardata;
		i = con_font_op(vc_cons[fg_console].d, op);
		if (i)
			return i;
		cfdarg.charheight = op->height;
		cfdarg.charcount = op->charcount;
		if (copy_to_user(user_cfd, &cfdarg, sizeof(struct consolefontdesc)))
			return -EFAULT;
		return 0;
		}
	}
	return -EINVAL;
}
static long vhost_vdpa_set_config_call(struct vhost_vdpa *v, u32 __user *argp)
{
	struct vdpa_callback cb;
	int fd;
	struct eventfd_ctx *ctx;

	cb.callback = vhost_vdpa_config_cb;
	cb.private = v->vdpa;
	if (copy_from_user(&fd, argp, sizeof(fd)))
		return  -EFAULT;

	ctx = fd == VHOST_FILE_UNBIND ? NULL : eventfd_ctx_fdget(fd);
	swap(ctx, v->config_ctx);

	if (!IS_ERR_OR_NULL(ctx))
		eventfd_ctx_put(ctx);

	if (IS_ERR(v->config_ctx))
		return PTR_ERR(v->config_ctx);

	v->vdpa->config->set_config_cb(v->vdpa, &cb);

	return 0;
}
static long vhost_vdpa_set_config(struct vhost_vdpa *v,
				  struct vhost_vdpa_config __user *c)
{
	struct vdpa_device *vdpa = v->vdpa;
	const struct vdpa_config_ops *ops = vdpa->config;
	struct vhost_vdpa_config config;
	unsigned long size = offsetof(struct vhost_vdpa_config, buf);
	u8 *buf;

	if (copy_from_user(&config, c, size))
		return -EFAULT;
	if (vhost_vdpa_config_validate(v, &config))
		return -EINVAL;

	buf = vmemdup_user(c->buf, config.len);
	if (IS_ERR(buf))
		return PTR_ERR(buf);

	ops->set_config(vdpa, config.off, buf, config.len);

	kvfree(buf);
	return 0;
}
static int vhost_vdpa_probe(struct vdpa_device *vdpa)
{
	const struct vdpa_config_ops *ops = vdpa->config;
	struct vhost_vdpa *v;
	int minor;
	int r;

	/* Currently, we only accept the network devices. */
	if (ops->get_device_id(vdpa) != VIRTIO_ID_NET)
		return -ENOTSUPP;

	v = kzalloc(sizeof(*v), GFP_KERNEL | __GFP_RETRY_MAYFAIL);
	if (!v)
		return -ENOMEM;

	minor = ida_simple_get(&vhost_vdpa_ida, 0,
			       VHOST_VDPA_DEV_MAX, GFP_KERNEL);
	if (minor < 0) {
		kfree(v);
		return minor;
	}

	atomic_set(&v->opened, 0);
	v->minor = minor;
	v->vdpa = vdpa;
	v->nvqs = vdpa->nvqs;
	v->virtio_id = ops->get_device_id(vdpa);

	device_initialize(&v->dev);
	v->dev.release = vhost_vdpa_release_dev;
	v->dev.parent = &vdpa->dev;
	v->dev.devt = MKDEV(MAJOR(vhost_vdpa_major), minor);
	v->vqs = kmalloc_array(v->nvqs, sizeof(struct vhost_virtqueue),
			       GFP_KERNEL);
	if (!v->vqs) {
		r = -ENOMEM;
		goto err;
	}

	r = dev_set_name(&v->dev, "vhost-vdpa-%u", minor);
	if (r)
		goto err;

	cdev_init(&v->cdev, &vhost_vdpa_fops);
	v->cdev.owner = THIS_MODULE;

	r = cdev_device_add(&v->cdev, &v->dev);
	if (r)
		goto err;

	init_completion(&v->completion);
	vdpa_set_drvdata(vdpa, v);

	return 0;

err:
	put_device(&v->dev);
	return r;
}
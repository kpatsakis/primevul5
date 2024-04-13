static int vhost_vdpa_config_validate(struct vhost_vdpa *v,
				      struct vhost_vdpa_config *c)
{
	long size = 0;

	switch (v->virtio_id) {
	case VIRTIO_ID_NET:
		size = sizeof(struct virtio_net_config);
		break;
	}

	if (c->len == 0)
		return -EINVAL;

	if (c->len > size - c->off)
		return -E2BIG;

	return 0;
}
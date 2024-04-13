static int vhost_vdpa_process_iotlb_msg(struct vhost_dev *dev,
					struct vhost_iotlb_msg *msg)
{
	struct vhost_vdpa *v = container_of(dev, struct vhost_vdpa, vdev);
	struct vdpa_device *vdpa = v->vdpa;
	const struct vdpa_config_ops *ops = vdpa->config;
	int r = 0;

	r = vhost_dev_check_owner(dev);
	if (r)
		return r;

	switch (msg->type) {
	case VHOST_IOTLB_UPDATE:
		r = vhost_vdpa_process_iotlb_update(v, msg);
		break;
	case VHOST_IOTLB_INVALIDATE:
		vhost_vdpa_unmap(v, msg->iova, msg->size);
		break;
	case VHOST_IOTLB_BATCH_BEGIN:
		v->in_batch = true;
		break;
	case VHOST_IOTLB_BATCH_END:
		if (v->in_batch && ops->set_map)
			ops->set_map(vdpa, dev->iotlb);
		v->in_batch = false;
		break;
	default:
		r = -EINVAL;
		break;
	}

	return r;
}
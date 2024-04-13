static long vhost_vdpa_vring_ioctl(struct vhost_vdpa *v, unsigned int cmd,
				   void __user *argp)
{
	struct vdpa_device *vdpa = v->vdpa;
	const struct vdpa_config_ops *ops = vdpa->config;
	struct vdpa_vq_state vq_state;
	struct vdpa_callback cb;
	struct vhost_virtqueue *vq;
	struct vhost_vring_state s;
	u32 idx;
	long r;

	r = get_user(idx, (u32 __user *)argp);
	if (r < 0)
		return r;

	if (idx >= v->nvqs)
		return -ENOBUFS;

	idx = array_index_nospec(idx, v->nvqs);
	vq = &v->vqs[idx];

	switch (cmd) {
	case VHOST_VDPA_SET_VRING_ENABLE:
		if (copy_from_user(&s, argp, sizeof(s)))
			return -EFAULT;
		ops->set_vq_ready(vdpa, idx, s.num);
		return 0;
	case VHOST_GET_VRING_BASE:
		r = ops->get_vq_state(v->vdpa, idx, &vq_state);
		if (r)
			return r;

		vq->last_avail_idx = vq_state.avail_index;
		break;
	}

	r = vhost_vring_ioctl(&v->vdev, cmd, argp);
	if (r)
		return r;

	switch (cmd) {
	case VHOST_SET_VRING_ADDR:
		if (ops->set_vq_address(vdpa, idx,
					(u64)(uintptr_t)vq->desc,
					(u64)(uintptr_t)vq->avail,
					(u64)(uintptr_t)vq->used))
			r = -EINVAL;
		break;

	case VHOST_SET_VRING_BASE:
		vq_state.avail_index = vq->last_avail_idx;
		if (ops->set_vq_state(vdpa, idx, &vq_state))
			r = -EINVAL;
		break;

	case VHOST_SET_VRING_CALL:
		if (vq->call_ctx.ctx) {
			cb.callback = vhost_vdpa_virtqueue_cb;
			cb.private = vq;
		} else {
			cb.callback = NULL;
			cb.private = NULL;
		}
		ops->set_vq_cb(vdpa, idx, &cb);
		vhost_vdpa_setup_vq_irq(v, idx);
		break;

	case VHOST_SET_VRING_NUM:
		ops->set_vq_num(vdpa, idx, vq->num);
		break;
	}

	return r;
}
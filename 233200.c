static int vhost_vdpa_open(struct inode *inode, struct file *filep)
{
	struct vhost_vdpa *v;
	struct vhost_dev *dev;
	struct vhost_virtqueue **vqs;
	int nvqs, i, r, opened;

	v = container_of(inode->i_cdev, struct vhost_vdpa, cdev);

	opened = atomic_cmpxchg(&v->opened, 0, 1);
	if (opened)
		return -EBUSY;

	nvqs = v->nvqs;
	vhost_vdpa_reset(v);

	vqs = kmalloc_array(nvqs, sizeof(*vqs), GFP_KERNEL);
	if (!vqs) {
		r = -ENOMEM;
		goto err;
	}

	dev = &v->vdev;
	for (i = 0; i < nvqs; i++) {
		vqs[i] = &v->vqs[i];
		vqs[i]->handle_kick = handle_vq_kick;
	}
	vhost_dev_init(dev, vqs, nvqs, 0, 0, 0, false,
		       vhost_vdpa_process_iotlb_msg);

	dev->iotlb = vhost_iotlb_alloc(0, 0);
	if (!dev->iotlb) {
		r = -ENOMEM;
		goto err_init_iotlb;
	}

	r = vhost_vdpa_alloc_domain(v);
	if (r)
		goto err_init_iotlb;

	vhost_vdpa_set_iova_range(v);

	filep->private_data = v;

	return 0;

err_init_iotlb:
	vhost_dev_cleanup(&v->vdev);
	kfree(vqs);
err:
	atomic_dec(&v->opened);
	return r;
}
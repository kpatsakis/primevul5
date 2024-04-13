static int vhost_vdpa_release(struct inode *inode, struct file *filep)
{
	struct vhost_vdpa *v = filep->private_data;
	struct vhost_dev *d = &v->vdev;

	mutex_lock(&d->mutex);
	filep->private_data = NULL;
	vhost_vdpa_reset(v);
	vhost_dev_stop(&v->vdev);
	vhost_vdpa_iotlb_free(v);
	vhost_vdpa_free_domain(v);
	vhost_vdpa_config_put(v);
	vhost_vdpa_clean_irq(v);
	vhost_dev_cleanup(&v->vdev);
	kfree(v->vdev.vqs);
	mutex_unlock(&d->mutex);

	atomic_dec(&v->opened);
	complete(&v->completion);

	return 0;
}
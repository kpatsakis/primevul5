static int vhost_vdpa_mmap(struct file *file, struct vm_area_struct *vma)
{
	struct vhost_vdpa *v = vma->vm_file->private_data;
	struct vdpa_device *vdpa = v->vdpa;
	const struct vdpa_config_ops *ops = vdpa->config;
	struct vdpa_notification_area notify;
	unsigned long index = vma->vm_pgoff;

	if (vma->vm_end - vma->vm_start != PAGE_SIZE)
		return -EINVAL;
	if ((vma->vm_flags & VM_SHARED) == 0)
		return -EINVAL;
	if (vma->vm_flags & VM_READ)
		return -EINVAL;
	if (index > 65535)
		return -EINVAL;
	if (!ops->get_vq_notification)
		return -ENOTSUPP;

	/* To be safe and easily modelled by userspace, We only
	 * support the doorbell which sits on the page boundary and
	 * does not share the page with other registers.
	 */
	notify = ops->get_vq_notification(vdpa, index);
	if (notify.addr & (PAGE_SIZE - 1))
		return -EINVAL;
	if (vma->vm_end - vma->vm_start != notify.size)
		return -ENOTSUPP;

	vma->vm_ops = &vhost_vdpa_vm_ops;
	return 0;
}
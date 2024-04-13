static int kvm_device_mmap(struct file *filp, struct vm_area_struct *vma)
{
	struct kvm_device *dev = filp->private_data;

	if (dev->ops->mmap)
		return dev->ops->mmap(dev, vma);

	return -ENODEV;
}
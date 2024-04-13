static inline long kvm_arch_vcpu_async_ioctl(struct file *filp,
					     unsigned int ioctl,
					     unsigned long arg)
{
	return -ENOIOCTLCMD;
}
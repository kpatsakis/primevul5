long kvm_arch_vcpu_async_ioctl(struct file *filp,
			       unsigned int ioctl, unsigned long arg)
{
	struct kvm_vcpu *vcpu = filp->private_data;
	void __user *argp = (void __user *)arg;

	switch (ioctl) {
	case KVM_S390_IRQ: {
		struct kvm_s390_irq s390irq;

		if (copy_from_user(&s390irq, argp, sizeof(s390irq)))
			return -EFAULT;
		return kvm_s390_inject_vcpu(vcpu, &s390irq);
	}
	case KVM_S390_INTERRUPT: {
		struct kvm_s390_interrupt s390int;
		struct kvm_s390_irq s390irq = {};

		if (copy_from_user(&s390int, argp, sizeof(s390int)))
			return -EFAULT;
		if (s390int_to_s390irq(&s390int, &s390irq))
			return -EINVAL;
		return kvm_s390_inject_vcpu(vcpu, &s390irq);
	}
	}
	return -ENOIOCTLCMD;
}
long kvm_arch_vm_ioctl(struct file *filp,
		       unsigned int ioctl, unsigned long arg)
{
	struct kvm *kvm = filp->private_data;
	void __user *argp = (void __user *)arg;
	struct kvm_device_attr attr;
	int r;

	switch (ioctl) {
	case KVM_S390_INTERRUPT: {
		struct kvm_s390_interrupt s390int;

		r = -EFAULT;
		if (copy_from_user(&s390int, argp, sizeof(s390int)))
			break;
		r = kvm_s390_inject_vm(kvm, &s390int);
		break;
	}
	case KVM_CREATE_IRQCHIP: {
		struct kvm_irq_routing_entry routing;

		r = -EINVAL;
		if (kvm->arch.use_irqchip) {
			/* Set up dummy routing. */
			memset(&routing, 0, sizeof(routing));
			r = kvm_set_irq_routing(kvm, &routing, 0, 0);
		}
		break;
	}
	case KVM_SET_DEVICE_ATTR: {
		r = -EFAULT;
		if (copy_from_user(&attr, (void __user *)arg, sizeof(attr)))
			break;
		r = kvm_s390_vm_set_attr(kvm, &attr);
		break;
	}
	case KVM_GET_DEVICE_ATTR: {
		r = -EFAULT;
		if (copy_from_user(&attr, (void __user *)arg, sizeof(attr)))
			break;
		r = kvm_s390_vm_get_attr(kvm, &attr);
		break;
	}
	case KVM_HAS_DEVICE_ATTR: {
		r = -EFAULT;
		if (copy_from_user(&attr, (void __user *)arg, sizeof(attr)))
			break;
		r = kvm_s390_vm_has_attr(kvm, &attr);
		break;
	}
	case KVM_S390_GET_SKEYS: {
		struct kvm_s390_skeys args;

		r = -EFAULT;
		if (copy_from_user(&args, argp,
				   sizeof(struct kvm_s390_skeys)))
			break;
		r = kvm_s390_get_skeys(kvm, &args);
		break;
	}
	case KVM_S390_SET_SKEYS: {
		struct kvm_s390_skeys args;

		r = -EFAULT;
		if (copy_from_user(&args, argp,
				   sizeof(struct kvm_s390_skeys)))
			break;
		r = kvm_s390_set_skeys(kvm, &args);
		break;
	}
	case KVM_S390_GET_CMMA_BITS: {
		struct kvm_s390_cmma_log args;

		r = -EFAULT;
		if (copy_from_user(&args, argp, sizeof(args)))
			break;
		mutex_lock(&kvm->slots_lock);
		r = kvm_s390_get_cmma_bits(kvm, &args);
		mutex_unlock(&kvm->slots_lock);
		if (!r) {
			r = copy_to_user(argp, &args, sizeof(args));
			if (r)
				r = -EFAULT;
		}
		break;
	}
	case KVM_S390_SET_CMMA_BITS: {
		struct kvm_s390_cmma_log args;

		r = -EFAULT;
		if (copy_from_user(&args, argp, sizeof(args)))
			break;
		mutex_lock(&kvm->slots_lock);
		r = kvm_s390_set_cmma_bits(kvm, &args);
		mutex_unlock(&kvm->slots_lock);
		break;
	}
	case KVM_S390_PV_COMMAND: {
		struct kvm_pv_cmd args;

		/* protvirt means user sigp */
		kvm->arch.user_cpu_state_ctrl = 1;
		r = 0;
		if (!is_prot_virt_host()) {
			r = -EINVAL;
			break;
		}
		if (copy_from_user(&args, argp, sizeof(args))) {
			r = -EFAULT;
			break;
		}
		if (args.flags) {
			r = -EINVAL;
			break;
		}
		mutex_lock(&kvm->lock);
		r = kvm_s390_handle_pv(kvm, &args);
		mutex_unlock(&kvm->lock);
		if (copy_to_user(argp, &args, sizeof(args))) {
			r = -EFAULT;
			break;
		}
		break;
	}
	default:
		r = -ENOTTY;
	}

	return r;
}
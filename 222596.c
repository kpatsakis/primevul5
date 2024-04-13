static int kvm_s390_set_mem_control(struct kvm *kvm, struct kvm_device_attr *attr)
{
	int ret;
	unsigned int idx;
	switch (attr->attr) {
	case KVM_S390_VM_MEM_ENABLE_CMMA:
		ret = -ENXIO;
		if (!sclp.has_cmma)
			break;

		VM_EVENT(kvm, 3, "%s", "ENABLE: CMMA support");
		mutex_lock(&kvm->lock);
		if (kvm->created_vcpus)
			ret = -EBUSY;
		else if (kvm->mm->context.allow_gmap_hpage_1m)
			ret = -EINVAL;
		else {
			kvm->arch.use_cmma = 1;
			/* Not compatible with cmma. */
			kvm->arch.use_pfmfi = 0;
			ret = 0;
		}
		mutex_unlock(&kvm->lock);
		break;
	case KVM_S390_VM_MEM_CLR_CMMA:
		ret = -ENXIO;
		if (!sclp.has_cmma)
			break;
		ret = -EINVAL;
		if (!kvm->arch.use_cmma)
			break;

		VM_EVENT(kvm, 3, "%s", "RESET: CMMA states");
		mutex_lock(&kvm->lock);
		idx = srcu_read_lock(&kvm->srcu);
		s390_reset_cmma(kvm->arch.gmap->mm);
		srcu_read_unlock(&kvm->srcu, idx);
		mutex_unlock(&kvm->lock);
		ret = 0;
		break;
	case KVM_S390_VM_MEM_LIMIT_SIZE: {
		unsigned long new_limit;

		if (kvm_is_ucontrol(kvm))
			return -EINVAL;

		if (get_user(new_limit, (u64 __user *)attr->addr))
			return -EFAULT;

		if (kvm->arch.mem_limit != KVM_S390_NO_MEM_LIMIT &&
		    new_limit > kvm->arch.mem_limit)
			return -E2BIG;

		if (!new_limit)
			return -EINVAL;

		/* gmap_create takes last usable address */
		if (new_limit != KVM_S390_NO_MEM_LIMIT)
			new_limit -= 1;

		ret = -EBUSY;
		mutex_lock(&kvm->lock);
		if (!kvm->created_vcpus) {
			/* gmap_create will round the limit up */
			struct gmap *new = gmap_create(current->mm, new_limit);

			if (!new) {
				ret = -ENOMEM;
			} else {
				gmap_remove(kvm->arch.gmap);
				new->private = kvm;
				kvm->arch.gmap = new;
				ret = 0;
			}
		}
		mutex_unlock(&kvm->lock);
		VM_EVENT(kvm, 3, "SET: max guest address: %lu", new_limit);
		VM_EVENT(kvm, 3, "New guest asce: 0x%pK",
			 (void *) kvm->arch.gmap->asce);
		break;
	}
	default:
		ret = -ENXIO;
		break;
	}
	return ret;
}
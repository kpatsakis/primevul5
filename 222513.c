int kvm_vm_ioctl_enable_cap(struct kvm *kvm, struct kvm_enable_cap *cap)
{
	int r;

	if (cap->flags)
		return -EINVAL;

	switch (cap->cap) {
	case KVM_CAP_S390_IRQCHIP:
		VM_EVENT(kvm, 3, "%s", "ENABLE: CAP_S390_IRQCHIP");
		kvm->arch.use_irqchip = 1;
		r = 0;
		break;
	case KVM_CAP_S390_USER_SIGP:
		VM_EVENT(kvm, 3, "%s", "ENABLE: CAP_S390_USER_SIGP");
		kvm->arch.user_sigp = 1;
		r = 0;
		break;
	case KVM_CAP_S390_VECTOR_REGISTERS:
		mutex_lock(&kvm->lock);
		if (kvm->created_vcpus) {
			r = -EBUSY;
		} else if (MACHINE_HAS_VX) {
			set_kvm_facility(kvm->arch.model.fac_mask, 129);
			set_kvm_facility(kvm->arch.model.fac_list, 129);
			if (test_facility(134)) {
				set_kvm_facility(kvm->arch.model.fac_mask, 134);
				set_kvm_facility(kvm->arch.model.fac_list, 134);
			}
			if (test_facility(135)) {
				set_kvm_facility(kvm->arch.model.fac_mask, 135);
				set_kvm_facility(kvm->arch.model.fac_list, 135);
			}
			if (test_facility(148)) {
				set_kvm_facility(kvm->arch.model.fac_mask, 148);
				set_kvm_facility(kvm->arch.model.fac_list, 148);
			}
			if (test_facility(152)) {
				set_kvm_facility(kvm->arch.model.fac_mask, 152);
				set_kvm_facility(kvm->arch.model.fac_list, 152);
			}
			r = 0;
		} else
			r = -EINVAL;
		mutex_unlock(&kvm->lock);
		VM_EVENT(kvm, 3, "ENABLE: CAP_S390_VECTOR_REGISTERS %s",
			 r ? "(not available)" : "(success)");
		break;
	case KVM_CAP_S390_RI:
		r = -EINVAL;
		mutex_lock(&kvm->lock);
		if (kvm->created_vcpus) {
			r = -EBUSY;
		} else if (test_facility(64)) {
			set_kvm_facility(kvm->arch.model.fac_mask, 64);
			set_kvm_facility(kvm->arch.model.fac_list, 64);
			r = 0;
		}
		mutex_unlock(&kvm->lock);
		VM_EVENT(kvm, 3, "ENABLE: CAP_S390_RI %s",
			 r ? "(not available)" : "(success)");
		break;
	case KVM_CAP_S390_AIS:
		mutex_lock(&kvm->lock);
		if (kvm->created_vcpus) {
			r = -EBUSY;
		} else {
			set_kvm_facility(kvm->arch.model.fac_mask, 72);
			set_kvm_facility(kvm->arch.model.fac_list, 72);
			r = 0;
		}
		mutex_unlock(&kvm->lock);
		VM_EVENT(kvm, 3, "ENABLE: AIS %s",
			 r ? "(not available)" : "(success)");
		break;
	case KVM_CAP_S390_GS:
		r = -EINVAL;
		mutex_lock(&kvm->lock);
		if (kvm->created_vcpus) {
			r = -EBUSY;
		} else if (test_facility(133)) {
			set_kvm_facility(kvm->arch.model.fac_mask, 133);
			set_kvm_facility(kvm->arch.model.fac_list, 133);
			r = 0;
		}
		mutex_unlock(&kvm->lock);
		VM_EVENT(kvm, 3, "ENABLE: CAP_S390_GS %s",
			 r ? "(not available)" : "(success)");
		break;
	case KVM_CAP_S390_HPAGE_1M:
		mutex_lock(&kvm->lock);
		if (kvm->created_vcpus)
			r = -EBUSY;
		else if (!hpage || kvm->arch.use_cmma || kvm_is_ucontrol(kvm))
			r = -EINVAL;
		else {
			r = 0;
			down_write(&kvm->mm->mmap_sem);
			kvm->mm->context.allow_gmap_hpage_1m = 1;
			up_write(&kvm->mm->mmap_sem);
			/*
			 * We might have to create fake 4k page
			 * tables. To avoid that the hardware works on
			 * stale PGSTEs, we emulate these instructions.
			 */
			kvm->arch.use_skf = 0;
			kvm->arch.use_pfmfi = 0;
		}
		mutex_unlock(&kvm->lock);
		VM_EVENT(kvm, 3, "ENABLE: CAP_S390_HPAGE %s",
			 r ? "(not available)" : "(success)");
		break;
	case KVM_CAP_S390_USER_STSI:
		VM_EVENT(kvm, 3, "%s", "ENABLE: CAP_S390_USER_STSI");
		kvm->arch.user_stsi = 1;
		r = 0;
		break;
	case KVM_CAP_S390_USER_INSTR0:
		VM_EVENT(kvm, 3, "%s", "ENABLE: CAP_S390_USER_INSTR0");
		kvm->arch.user_instr0 = 1;
		icpt_operexc_on_all_vcpus(kvm);
		r = 0;
		break;
	default:
		r = -EINVAL;
		break;
	}
	return r;
}
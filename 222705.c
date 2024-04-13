static int kvm_arch_vcpu_ioctl_set_one_reg(struct kvm_vcpu *vcpu,
					   struct kvm_one_reg *reg)
{
	int r = -EINVAL;
	__u64 val;

	switch (reg->id) {
	case KVM_REG_S390_TODPR:
		r = get_user(vcpu->arch.sie_block->todpr,
			     (u32 __user *)reg->addr);
		break;
	case KVM_REG_S390_EPOCHDIFF:
		r = get_user(vcpu->arch.sie_block->epoch,
			     (u64 __user *)reg->addr);
		break;
	case KVM_REG_S390_CPU_TIMER:
		r = get_user(val, (u64 __user *)reg->addr);
		if (!r)
			kvm_s390_set_cpu_timer(vcpu, val);
		break;
	case KVM_REG_S390_CLOCK_COMP:
		r = get_user(vcpu->arch.sie_block->ckc,
			     (u64 __user *)reg->addr);
		break;
	case KVM_REG_S390_PFTOKEN:
		r = get_user(vcpu->arch.pfault_token,
			     (u64 __user *)reg->addr);
		if (vcpu->arch.pfault_token == KVM_S390_PFAULT_TOKEN_INVALID)
			kvm_clear_async_pf_completion_queue(vcpu);
		break;
	case KVM_REG_S390_PFCOMPARE:
		r = get_user(vcpu->arch.pfault_compare,
			     (u64 __user *)reg->addr);
		break;
	case KVM_REG_S390_PFSELECT:
		r = get_user(vcpu->arch.pfault_select,
			     (u64 __user *)reg->addr);
		break;
	case KVM_REG_S390_PP:
		r = get_user(vcpu->arch.sie_block->pp,
			     (u64 __user *)reg->addr);
		break;
	case KVM_REG_S390_GBEA:
		r = get_user(vcpu->arch.sie_block->gbea,
			     (u64 __user *)reg->addr);
		break;
	default:
		break;
	}

	return r;
}
static int kvm_arch_vcpu_ioctl_get_one_reg(struct kvm_vcpu *vcpu,
					   struct kvm_one_reg *reg)
{
	int r = -EINVAL;

	switch (reg->id) {
	case KVM_REG_S390_TODPR:
		r = put_user(vcpu->arch.sie_block->todpr,
			     (u32 __user *)reg->addr);
		break;
	case KVM_REG_S390_EPOCHDIFF:
		r = put_user(vcpu->arch.sie_block->epoch,
			     (u64 __user *)reg->addr);
		break;
	case KVM_REG_S390_CPU_TIMER:
		r = put_user(kvm_s390_get_cpu_timer(vcpu),
			     (u64 __user *)reg->addr);
		break;
	case KVM_REG_S390_CLOCK_COMP:
		r = put_user(vcpu->arch.sie_block->ckc,
			     (u64 __user *)reg->addr);
		break;
	case KVM_REG_S390_PFTOKEN:
		r = put_user(vcpu->arch.pfault_token,
			     (u64 __user *)reg->addr);
		break;
	case KVM_REG_S390_PFCOMPARE:
		r = put_user(vcpu->arch.pfault_compare,
			     (u64 __user *)reg->addr);
		break;
	case KVM_REG_S390_PFSELECT:
		r = put_user(vcpu->arch.pfault_select,
			     (u64 __user *)reg->addr);
		break;
	case KVM_REG_S390_PP:
		r = put_user(vcpu->arch.sie_block->pp,
			     (u64 __user *)reg->addr);
		break;
	case KVM_REG_S390_GBEA:
		r = put_user(vcpu->arch.sie_block->gbea,
			     (u64 __user *)reg->addr);
		break;
	default:
		break;
	}

	return r;
}
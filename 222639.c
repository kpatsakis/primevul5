static long kvm_s390_guest_memsida_op(struct kvm_vcpu *vcpu,
				      struct kvm_s390_mem_op *mop)
{
	int r, srcu_idx;

	srcu_idx = srcu_read_lock(&vcpu->kvm->srcu);

	switch (mop->op) {
	case KVM_S390_MEMOP_LOGICAL_READ:
	case KVM_S390_MEMOP_LOGICAL_WRITE:
		r = kvm_s390_guest_mem_op(vcpu, mop);
		break;
	case KVM_S390_MEMOP_SIDA_READ:
	case KVM_S390_MEMOP_SIDA_WRITE:
		/* we are locked against sida going away by the vcpu->mutex */
		r = kvm_s390_guest_sida_op(vcpu, mop);
		break;
	default:
		r = -EINVAL;
	}

	srcu_read_unlock(&vcpu->kvm->srcu, srcu_idx);
	return r;
}
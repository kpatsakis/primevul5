static long kvm_s390_guest_mem_op(struct kvm_vcpu *vcpu,
				  struct kvm_s390_mem_op *mop)
{
	void __user *uaddr = (void __user *)mop->buf;
	void *tmpbuf = NULL;
	int r = 0;
	const u64 supported_flags = KVM_S390_MEMOP_F_INJECT_EXCEPTION
				    | KVM_S390_MEMOP_F_CHECK_ONLY;

	if (mop->flags & ~supported_flags || mop->ar >= NUM_ACRS || !mop->size)
		return -EINVAL;

	if (mop->size > MEM_OP_MAX_SIZE)
		return -E2BIG;

	if (kvm_s390_pv_cpu_is_protected(vcpu))
		return -EINVAL;

	if (!(mop->flags & KVM_S390_MEMOP_F_CHECK_ONLY)) {
		tmpbuf = vmalloc(mop->size);
		if (!tmpbuf)
			return -ENOMEM;
	}

	switch (mop->op) {
	case KVM_S390_MEMOP_LOGICAL_READ:
		if (mop->flags & KVM_S390_MEMOP_F_CHECK_ONLY) {
			r = check_gva_range(vcpu, mop->gaddr, mop->ar,
					    mop->size, GACC_FETCH);
			break;
		}
		r = read_guest(vcpu, mop->gaddr, mop->ar, tmpbuf, mop->size);
		if (r == 0) {
			if (copy_to_user(uaddr, tmpbuf, mop->size))
				r = -EFAULT;
		}
		break;
	case KVM_S390_MEMOP_LOGICAL_WRITE:
		if (mop->flags & KVM_S390_MEMOP_F_CHECK_ONLY) {
			r = check_gva_range(vcpu, mop->gaddr, mop->ar,
					    mop->size, GACC_STORE);
			break;
		}
		if (copy_from_user(tmpbuf, uaddr, mop->size)) {
			r = -EFAULT;
			break;
		}
		r = write_guest(vcpu, mop->gaddr, mop->ar, tmpbuf, mop->size);
		break;
	}

	if (r > 0 && (mop->flags & KVM_S390_MEMOP_F_INJECT_EXCEPTION) != 0)
		kvm_s390_inject_prog_irq(vcpu, &vcpu->arch.pgm);

	vfree(tmpbuf);
	return r;
}
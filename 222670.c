static int kvm_s390_set_cmma_bits(struct kvm *kvm,
				  const struct kvm_s390_cmma_log *args)
{
	unsigned long hva, mask, pgstev, i;
	uint8_t *bits;
	int srcu_idx, r = 0;

	mask = args->mask;

	if (!kvm->arch.use_cmma)
		return -ENXIO;
	/* invalid/unsupported flags */
	if (args->flags != 0)
		return -EINVAL;
	/* Enforce sane limit on memory allocation */
	if (args->count > KVM_S390_CMMA_SIZE_MAX)
		return -EINVAL;
	/* Nothing to do */
	if (args->count == 0)
		return 0;

	bits = vmalloc(array_size(sizeof(*bits), args->count));
	if (!bits)
		return -ENOMEM;

	r = copy_from_user(bits, (void __user *)args->values, args->count);
	if (r) {
		r = -EFAULT;
		goto out;
	}

	down_read(&kvm->mm->mmap_sem);
	srcu_idx = srcu_read_lock(&kvm->srcu);
	for (i = 0; i < args->count; i++) {
		hva = gfn_to_hva(kvm, args->start_gfn + i);
		if (kvm_is_error_hva(hva)) {
			r = -EFAULT;
			break;
		}

		pgstev = bits[i];
		pgstev = pgstev << 24;
		mask &= _PGSTE_GPS_USAGE_MASK | _PGSTE_GPS_NODAT;
		set_pgste_bits(kvm->mm, hva, mask, pgstev);
	}
	srcu_read_unlock(&kvm->srcu, srcu_idx);
	up_read(&kvm->mm->mmap_sem);

	if (!kvm->mm->context.uses_cmm) {
		down_write(&kvm->mm->mmap_sem);
		kvm->mm->context.uses_cmm = 1;
		up_write(&kvm->mm->mmap_sem);
	}
out:
	vfree(bits);
	return r;
}
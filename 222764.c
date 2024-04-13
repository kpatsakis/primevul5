static int kvm_s390_get_cmma_bits(struct kvm *kvm,
				  struct kvm_s390_cmma_log *args)
{
	unsigned long bufsize;
	int srcu_idx, peek, ret;
	u8 *values;

	if (!kvm->arch.use_cmma)
		return -ENXIO;
	/* Invalid/unsupported flags were specified */
	if (args->flags & ~KVM_S390_CMMA_PEEK)
		return -EINVAL;
	/* Migration mode query, and we are not doing a migration */
	peek = !!(args->flags & KVM_S390_CMMA_PEEK);
	if (!peek && !kvm->arch.migration_mode)
		return -EINVAL;
	/* CMMA is disabled or was not used, or the buffer has length zero */
	bufsize = min(args->count, KVM_S390_CMMA_SIZE_MAX);
	if (!bufsize || !kvm->mm->context.uses_cmm) {
		memset(args, 0, sizeof(*args));
		return 0;
	}
	/* We are not peeking, and there are no dirty pages */
	if (!peek && !atomic64_read(&kvm->arch.cmma_dirty_pages)) {
		memset(args, 0, sizeof(*args));
		return 0;
	}

	values = vmalloc(bufsize);
	if (!values)
		return -ENOMEM;

	down_read(&kvm->mm->mmap_sem);
	srcu_idx = srcu_read_lock(&kvm->srcu);
	if (peek)
		ret = kvm_s390_peek_cmma(kvm, args, values, bufsize);
	else
		ret = kvm_s390_get_cmma(kvm, args, values, bufsize);
	srcu_read_unlock(&kvm->srcu, srcu_idx);
	up_read(&kvm->mm->mmap_sem);

	if (kvm->arch.migration_mode)
		args->remaining = atomic64_read(&kvm->arch.cmma_dirty_pages);
	else
		args->remaining = 0;

	if (copy_to_user((void __user *)args->values, values, args->count))
		ret = -EFAULT;

	vfree(values);
	return ret;
}
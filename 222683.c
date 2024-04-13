static long kvm_s390_get_skeys(struct kvm *kvm, struct kvm_s390_skeys *args)
{
	uint8_t *keys;
	uint64_t hva;
	int srcu_idx, i, r = 0;

	if (args->flags != 0)
		return -EINVAL;

	/* Is this guest using storage keys? */
	if (!mm_uses_skeys(current->mm))
		return KVM_S390_GET_SKEYS_NONE;

	/* Enforce sane limit on memory allocation */
	if (args->count < 1 || args->count > KVM_S390_SKEYS_MAX)
		return -EINVAL;

	keys = kvmalloc_array(args->count, sizeof(uint8_t), GFP_KERNEL);
	if (!keys)
		return -ENOMEM;

	down_read(&current->mm->mmap_sem);
	srcu_idx = srcu_read_lock(&kvm->srcu);
	for (i = 0; i < args->count; i++) {
		hva = gfn_to_hva(kvm, args->start_gfn + i);
		if (kvm_is_error_hva(hva)) {
			r = -EFAULT;
			break;
		}

		r = get_guest_storage_key(current->mm, hva, &keys[i]);
		if (r)
			break;
	}
	srcu_read_unlock(&kvm->srcu, srcu_idx);
	up_read(&current->mm->mmap_sem);

	if (!r) {
		r = copy_to_user((uint8_t __user *)args->skeydata_addr, keys,
				 sizeof(uint8_t) * args->count);
		if (r)
			r = -EFAULT;
	}

	kvfree(keys);
	return r;
}
static long kvm_s390_set_skeys(struct kvm *kvm, struct kvm_s390_skeys *args)
{
	uint8_t *keys;
	uint64_t hva;
	int srcu_idx, i, r = 0;
	bool unlocked;

	if (args->flags != 0)
		return -EINVAL;

	/* Enforce sane limit on memory allocation */
	if (args->count < 1 || args->count > KVM_S390_SKEYS_MAX)
		return -EINVAL;

	keys = kvmalloc_array(args->count, sizeof(uint8_t), GFP_KERNEL);
	if (!keys)
		return -ENOMEM;

	r = copy_from_user(keys, (uint8_t __user *)args->skeydata_addr,
			   sizeof(uint8_t) * args->count);
	if (r) {
		r = -EFAULT;
		goto out;
	}

	/* Enable storage key handling for the guest */
	r = s390_enable_skey();
	if (r)
		goto out;

	i = 0;
	down_read(&current->mm->mmap_sem);
	srcu_idx = srcu_read_lock(&kvm->srcu);
        while (i < args->count) {
		unlocked = false;
		hva = gfn_to_hva(kvm, args->start_gfn + i);
		if (kvm_is_error_hva(hva)) {
			r = -EFAULT;
			break;
		}

		/* Lowest order bit is reserved */
		if (keys[i] & 0x01) {
			r = -EINVAL;
			break;
		}

		r = set_guest_storage_key(current->mm, hva, keys[i], 0);
		if (r) {
			r = fixup_user_fault(current, current->mm, hva,
					     FAULT_FLAG_WRITE, &unlocked);
			if (r)
				break;
		}
		if (!r)
			i++;
	}
	srcu_read_unlock(&kvm->srcu, srcu_idx);
	up_read(&current->mm->mmap_sem);
out:
	kvfree(keys);
	return r;
}
int kvm_read_guest_offset_cached(struct kvm *kvm, struct gfn_to_hva_cache *ghc,
				 void *data, unsigned int offset,
				 unsigned long len)
{
	struct kvm_memslots *slots = kvm_memslots(kvm);
	int r;
	gpa_t gpa = ghc->gpa + offset;

	BUG_ON(len + offset > ghc->len);

	if (slots->generation != ghc->generation) {
		if (__kvm_gfn_to_hva_cache_init(slots, ghc, ghc->gpa, ghc->len))
			return -EFAULT;
	}

	if (kvm_is_error_hva(ghc->hva))
		return -EFAULT;

	if (unlikely(!ghc->memslot))
		return kvm_read_guest(kvm, gpa, data, len);

	r = __copy_from_user(data, (void __user *)ghc->hva + offset, len);
	if (r)
		return -EFAULT;

	return 0;
}
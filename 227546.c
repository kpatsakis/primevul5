int kvm_write_guest_offset_cached(struct kvm *kvm, struct gfn_to_hva_cache *ghc,
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
		return kvm_write_guest(kvm, gpa, data, len);

	r = __copy_to_user((void __user *)ghc->hva + offset, data, len);
	if (r)
		return -EFAULT;
	mark_page_dirty_in_slot(kvm, ghc->memslot, gpa >> PAGE_SHIFT);

	return 0;
}
int kvm_clear_guest(struct kvm *kvm, gpa_t gpa, unsigned long len)
{
	const void *zero_page = (const void *) __va(page_to_phys(ZERO_PAGE(0)));
	gfn_t gfn = gpa >> PAGE_SHIFT;
	int seg;
	int offset = offset_in_page(gpa);
	int ret;

	while ((seg = next_segment(len, offset)) != 0) {
		ret = kvm_write_guest_page(kvm, gfn, zero_page, offset, len);
		if (ret < 0)
			return ret;
		offset = 0;
		len -= seg;
		++gfn;
	}
	return 0;
}
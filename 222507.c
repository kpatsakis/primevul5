static int kvm_s390_peek_cmma(struct kvm *kvm, struct kvm_s390_cmma_log *args,
			      u8 *res, unsigned long bufsize)
{
	unsigned long pgstev, hva, cur_gfn = args->start_gfn;

	args->count = 0;
	while (args->count < bufsize) {
		hva = gfn_to_hva(kvm, cur_gfn);
		/*
		 * We return an error if the first value was invalid, but we
		 * return successfully if at least one value was copied.
		 */
		if (kvm_is_error_hva(hva))
			return args->count ? 0 : -EFAULT;
		if (get_pgste(kvm->mm, hva, &pgstev) < 0)
			pgstev = 0;
		res[args->count++] = (pgstev >> 24) & 0x43;
		cur_gfn++;
	}

	return 0;
}
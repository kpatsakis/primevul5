static int check_svme_pa(struct x86_emulate_ctxt *ctxt)
{
	u64 rax = ctxt->regs[VCPU_REGS_RAX];

	/* Valid physical address? */
	if (rax & 0xffff000000000000ULL)
		return emulate_gp(ctxt, 0);

	return check_svme(ctxt);
}
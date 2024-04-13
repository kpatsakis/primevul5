static int em_cmpxchg8b(struct x86_emulate_ctxt *ctxt)
{
	u64 old = ctxt->dst.orig_val64;

	if (((u32) (old >> 0) != (u32) ctxt->regs[VCPU_REGS_RAX]) ||
	    ((u32) (old >> 32) != (u32) ctxt->regs[VCPU_REGS_RDX])) {
		ctxt->regs[VCPU_REGS_RAX] = (u32) (old >> 0);
		ctxt->regs[VCPU_REGS_RDX] = (u32) (old >> 32);
		ctxt->eflags &= ~EFLG_ZF;
	} else {
		ctxt->dst.val64 = ((u64)ctxt->regs[VCPU_REGS_RCX] << 32) |
			(u32) ctxt->regs[VCPU_REGS_RBX];

		ctxt->eflags |= EFLG_ZF;
	}
	return X86EMUL_CONTINUE;
}
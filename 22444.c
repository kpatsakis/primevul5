static int load_state_from_tss32(struct x86_emulate_ctxt *ctxt,
				 struct tss_segment_32 *tss)
{
	int ret;

	if (ctxt->ops->set_cr(ctxt, 3, tss->cr3))
		return emulate_gp(ctxt, 0);
	ctxt->_eip = tss->eip;
	ctxt->eflags = tss->eflags | 2;
	ctxt->regs[VCPU_REGS_RAX] = tss->eax;
	ctxt->regs[VCPU_REGS_RCX] = tss->ecx;
	ctxt->regs[VCPU_REGS_RDX] = tss->edx;
	ctxt->regs[VCPU_REGS_RBX] = tss->ebx;
	ctxt->regs[VCPU_REGS_RSP] = tss->esp;
	ctxt->regs[VCPU_REGS_RBP] = tss->ebp;
	ctxt->regs[VCPU_REGS_RSI] = tss->esi;
	ctxt->regs[VCPU_REGS_RDI] = tss->edi;

	/*
	 * SDM says that segment selectors are loaded before segment
	 * descriptors
	 */
	set_segment_selector(ctxt, tss->ldt_selector, VCPU_SREG_LDTR);
	set_segment_selector(ctxt, tss->es, VCPU_SREG_ES);
	set_segment_selector(ctxt, tss->cs, VCPU_SREG_CS);
	set_segment_selector(ctxt, tss->ss, VCPU_SREG_SS);
	set_segment_selector(ctxt, tss->ds, VCPU_SREG_DS);
	set_segment_selector(ctxt, tss->fs, VCPU_SREG_FS);
	set_segment_selector(ctxt, tss->gs, VCPU_SREG_GS);

	/*
	 * Now load segment descriptors. If fault happenes at this stage
	 * it is handled in a context of new task
	 */
	ret = load_segment_descriptor(ctxt, tss->ldt_selector, VCPU_SREG_LDTR);
	if (ret != X86EMUL_CONTINUE)
		return ret;
	ret = load_segment_descriptor(ctxt, tss->es, VCPU_SREG_ES);
	if (ret != X86EMUL_CONTINUE)
		return ret;
	ret = load_segment_descriptor(ctxt, tss->cs, VCPU_SREG_CS);
	if (ret != X86EMUL_CONTINUE)
		return ret;
	ret = load_segment_descriptor(ctxt, tss->ss, VCPU_SREG_SS);
	if (ret != X86EMUL_CONTINUE)
		return ret;
	ret = load_segment_descriptor(ctxt, tss->ds, VCPU_SREG_DS);
	if (ret != X86EMUL_CONTINUE)
		return ret;
	ret = load_segment_descriptor(ctxt, tss->fs, VCPU_SREG_FS);
	if (ret != X86EMUL_CONTINUE)
		return ret;
	ret = load_segment_descriptor(ctxt, tss->gs, VCPU_SREG_GS);
	if (ret != X86EMUL_CONTINUE)
		return ret;

	return X86EMUL_CONTINUE;
}
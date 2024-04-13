static void save_state_to_tss32(struct x86_emulate_ctxt *ctxt,
				struct tss_segment_32 *tss)
{
	tss->cr3 = ctxt->ops->get_cr(ctxt, 3);
	tss->eip = ctxt->_eip;
	tss->eflags = ctxt->eflags;
	tss->eax = ctxt->regs[VCPU_REGS_RAX];
	tss->ecx = ctxt->regs[VCPU_REGS_RCX];
	tss->edx = ctxt->regs[VCPU_REGS_RDX];
	tss->ebx = ctxt->regs[VCPU_REGS_RBX];
	tss->esp = ctxt->regs[VCPU_REGS_RSP];
	tss->ebp = ctxt->regs[VCPU_REGS_RBP];
	tss->esi = ctxt->regs[VCPU_REGS_RSI];
	tss->edi = ctxt->regs[VCPU_REGS_RDI];

	tss->es = get_segment_selector(ctxt, VCPU_SREG_ES);
	tss->cs = get_segment_selector(ctxt, VCPU_SREG_CS);
	tss->ss = get_segment_selector(ctxt, VCPU_SREG_SS);
	tss->ds = get_segment_selector(ctxt, VCPU_SREG_DS);
	tss->fs = get_segment_selector(ctxt, VCPU_SREG_FS);
	tss->gs = get_segment_selector(ctxt, VCPU_SREG_GS);
	tss->ldt_selector = get_segment_selector(ctxt, VCPU_SREG_LDTR);
}
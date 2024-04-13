static void save_state_to_tss16(struct x86_emulate_ctxt *ctxt,
				struct tss_segment_16 *tss)
{
	tss->ip = ctxt->_eip;
	tss->flag = ctxt->eflags;
	tss->ax = ctxt->regs[VCPU_REGS_RAX];
	tss->cx = ctxt->regs[VCPU_REGS_RCX];
	tss->dx = ctxt->regs[VCPU_REGS_RDX];
	tss->bx = ctxt->regs[VCPU_REGS_RBX];
	tss->sp = ctxt->regs[VCPU_REGS_RSP];
	tss->bp = ctxt->regs[VCPU_REGS_RBP];
	tss->si = ctxt->regs[VCPU_REGS_RSI];
	tss->di = ctxt->regs[VCPU_REGS_RDI];

	tss->es = get_segment_selector(ctxt, VCPU_SREG_ES);
	tss->cs = get_segment_selector(ctxt, VCPU_SREG_CS);
	tss->ss = get_segment_selector(ctxt, VCPU_SREG_SS);
	tss->ds = get_segment_selector(ctxt, VCPU_SREG_DS);
	tss->ldt = get_segment_selector(ctxt, VCPU_SREG_LDTR);
}
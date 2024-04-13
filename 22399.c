static void string_addr_inc(struct x86_emulate_ctxt *ctxt, unsigned seg,
			    int reg, struct operand *op)
{
	int df = (ctxt->eflags & EFLG_DF) ? -1 : 1;

	register_address_increment(ctxt, &ctxt->regs[reg], df * op->bytes);
	op->addr.mem.ea = register_address(ctxt, ctxt->regs[reg]);
	op->addr.mem.seg = seg;
}
static int em_ret_far(struct x86_emulate_ctxt *ctxt)
{
	int rc;
	unsigned long cs;

	rc = emulate_pop(ctxt, &ctxt->_eip, ctxt->op_bytes);
	if (rc != X86EMUL_CONTINUE)
		return rc;
	if (ctxt->op_bytes == 4)
		ctxt->_eip = (u32)ctxt->_eip;
	rc = emulate_pop(ctxt, &cs, ctxt->op_bytes);
	if (rc != X86EMUL_CONTINUE)
		return rc;
	rc = load_segment_descriptor(ctxt, (u16)cs, VCPU_SREG_CS);
	return rc;
}
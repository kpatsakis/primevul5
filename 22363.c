int emulator_task_switch(struct x86_emulate_ctxt *ctxt,
			 u16 tss_selector, int reason,
			 bool has_error_code, u32 error_code)
{
	int rc;

	ctxt->_eip = ctxt->eip;
	ctxt->dst.type = OP_NONE;

	rc = emulator_do_task_switch(ctxt, tss_selector, reason,
				     has_error_code, error_code);

	if (rc == X86EMUL_CONTINUE)
		ctxt->eip = ctxt->_eip;

	return (rc == X86EMUL_UNHANDLEABLE) ? EMULATION_FAILED : EMULATION_OK;
}
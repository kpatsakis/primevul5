static int em_smsw(struct x86_emulate_ctxt *ctxt)
{
	ctxt->dst.bytes = 2;
	ctxt->dst.val = ctxt->ops->get_cr(ctxt, 0);
	return X86EMUL_CONTINUE;
}
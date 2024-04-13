static int em_imul_3op(struct x86_emulate_ctxt *ctxt)
{
	ctxt->dst.val = ctxt->src2.val;
	return em_imul(ctxt);
}
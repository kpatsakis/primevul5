static int em_bt(struct x86_emulate_ctxt *ctxt)
{
	/* Disable writeback. */
	ctxt->dst.type = OP_NONE;
	/* only subword offset */
	ctxt->src.val &= (ctxt->dst.bytes << 3) - 1;

	emulate_2op_SrcV_nobyte(ctxt, "bt");
	return X86EMUL_CONTINUE;
}
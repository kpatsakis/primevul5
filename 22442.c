static int writeback(struct x86_emulate_ctxt *ctxt)
{
	int rc;

	switch (ctxt->dst.type) {
	case OP_REG:
		write_register_operand(&ctxt->dst);
		break;
	case OP_MEM:
		if (ctxt->lock_prefix)
			rc = segmented_cmpxchg(ctxt,
					       ctxt->dst.addr.mem,
					       &ctxt->dst.orig_val,
					       &ctxt->dst.val,
					       ctxt->dst.bytes);
		else
			rc = segmented_write(ctxt,
					     ctxt->dst.addr.mem,
					     &ctxt->dst.val,
					     ctxt->dst.bytes);
		if (rc != X86EMUL_CONTINUE)
			return rc;
		break;
	case OP_XMM:
		write_sse_reg(ctxt, &ctxt->dst.vec_val, ctxt->dst.addr.xmm);
		break;
	case OP_NONE:
		/* no writeback */
		break;
	default:
		break;
	}
	return X86EMUL_CONTINUE;
}
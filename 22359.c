static unsigned seg_override(struct x86_emulate_ctxt *ctxt)
{
	if (!ctxt->has_seg_override)
		return 0;

	return ctxt->seg_override;
}
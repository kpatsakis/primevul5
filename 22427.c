static void set_seg_override(struct x86_emulate_ctxt *ctxt, int seg)
{
	ctxt->has_seg_override = true;
	ctxt->seg_override = seg;
}
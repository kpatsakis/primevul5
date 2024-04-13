static int read_segment_descriptor(struct x86_emulate_ctxt *ctxt,
				   u16 selector, struct desc_struct *desc)
{
	struct desc_ptr dt;
	u16 index = selector >> 3;
	ulong addr;

	get_descriptor_table_ptr(ctxt, selector, &dt);

	if (dt.size < index * 8 + 7)
		return emulate_gp(ctxt, selector & 0xfffc);

	addr = dt.address + index * 8;
	return ctxt->ops->read_std(ctxt, addr, desc, sizeof *desc,
				   &ctxt->exception);
}
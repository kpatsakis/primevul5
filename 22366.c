register_address_increment(struct x86_emulate_ctxt *ctxt, unsigned long *reg, int inc)
{
	if (ctxt->ad_bytes == sizeof(unsigned long))
		*reg += inc;
	else
		*reg = (*reg & ~ad_mask(ctxt)) | ((*reg + inc) & ad_mask(ctxt));
}
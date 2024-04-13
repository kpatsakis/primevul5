struct dpp *dcn20_dpp_create(
	struct dc_context *ctx,
	uint32_t inst)
{
	struct dcn20_dpp *dpp =
		kzalloc(sizeof(struct dcn20_dpp), GFP_KERNEL);

	if (!dpp)
		return NULL;

	if (dpp2_construct(dpp, ctx, inst,
			&tf_regs[inst], &tf_shift, &tf_mask))
		return &dpp->base;

	BREAK_TO_DEBUGGER();
	kfree(dpp);
	return NULL;
}
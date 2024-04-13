bool dcn20_mmhubbub_create(struct dc_context *ctx, struct resource_pool *pool)
{
	int i;
	uint32_t pipe_count = pool->res_cap->num_dwb;

	ASSERT(pipe_count > 0);

	for (i = 0; i < pipe_count; i++) {
		struct dcn20_mmhubbub *mcif_wb20 = kzalloc(sizeof(struct dcn20_mmhubbub),
						    GFP_KERNEL);

		if (!mcif_wb20) {
			dm_error("DC: failed to create mcif_wb20!\n");
			return false;
		}

		dcn20_mmhubbub_construct(mcif_wb20, ctx,
				&mcif_wb20_regs[i],
				&mcif_wb20_shift,
				&mcif_wb20_mask,
				i);

		pool->mcif_wb[i] = &mcif_wb20->base;
	}
	return true;
}
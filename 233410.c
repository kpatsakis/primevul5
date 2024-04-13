bool dcn20_dwbc_create(struct dc_context *ctx, struct resource_pool *pool)
{
	int i;
	uint32_t pipe_count = pool->res_cap->num_dwb;

	ASSERT(pipe_count > 0);

	for (i = 0; i < pipe_count; i++) {
		struct dcn20_dwbc *dwbc20 = kzalloc(sizeof(struct dcn20_dwbc),
						    GFP_KERNEL);

		if (!dwbc20) {
			dm_error("DC: failed to create dwbc20!\n");
			return false;
		}
		dcn20_dwbc_construct(dwbc20, ctx,
				&dwbc20_regs[i],
				&dwbc20_shift,
				&dwbc20_mask,
				i);
		pool->dwbc[i] = &dwbc20->base;
	}
	return true;
}
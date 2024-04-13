static void release_dsc(struct resource_context *res_ctx,
			const struct resource_pool *pool,
			struct display_stream_compressor **dsc)
{
	int i;

	for (i = 0; i < pool->res_cap->num_dsc; i++)
		if (pool->dscs[i] == *dsc) {
			res_ctx->is_dsc_acquired[i] = false;
			*dsc = NULL;
			break;
		}
}
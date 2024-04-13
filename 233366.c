static void acquire_dsc(struct resource_context *res_ctx,
			const struct resource_pool *pool,
			struct display_stream_compressor **dsc)
{
	int i;

	ASSERT(*dsc == NULL);
	*dsc = NULL;

	/* Find first free DSC */
	for (i = 0; i < pool->res_cap->num_dsc; i++)
		if (!res_ctx->is_dsc_acquired[i]) {
			*dsc = pool->dscs[i];
			res_ctx->is_dsc_acquired[i] = true;
			break;
		}
}
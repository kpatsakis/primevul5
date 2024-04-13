struct resource_pool *dce80_create_resource_pool(
	uint8_t num_virtual_links,
	struct dc *dc)
{
	struct dce110_resource_pool *pool =
		kzalloc(sizeof(struct dce110_resource_pool), GFP_KERNEL);

	if (!pool)
		return NULL;

	if (dce80_construct(num_virtual_links, dc, pool))
		return &pool->base;

	BREAK_TO_DEBUGGER();
	return NULL;
}
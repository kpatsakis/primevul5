static void dcn20_destroy_resource_pool(struct resource_pool **pool)
{
	struct dcn20_resource_pool *dcn20_pool = TO_DCN20_RES_POOL(*pool);

	destruct(dcn20_pool);
	kfree(dcn20_pool);
	*pool = NULL;
}
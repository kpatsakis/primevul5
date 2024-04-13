
static void bfq_slab_kill(void)
{
	kmem_cache_destroy(bfq_pool);
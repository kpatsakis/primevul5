
static void __exit bfq_exit(void)
{
	elv_unregister(&iosched_bfq_mq);
#ifdef CONFIG_BFQ_GROUP_IOSCHED
	blkcg_policy_unregister(&blkcg_policy_bfq);
#endif
	bfq_slab_kill();
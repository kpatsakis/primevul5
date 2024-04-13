
static int __init bfq_init(void)
{
	int ret;

#ifdef CONFIG_BFQ_GROUP_IOSCHED
	ret = blkcg_policy_register(&blkcg_policy_bfq);
	if (ret)
		return ret;
#endif

	ret = -ENOMEM;
	if (bfq_slab_setup())
		goto err_pol_unreg;

	/*
	 * Times to load large popular applications for the typical
	 * systems installed on the reference devices (see the
	 * comments before the definition of the next
	 * array). Actually, we use slightly lower values, as the
	 * estimated peak rate tends to be smaller than the actual
	 * peak rate.  The reason for this last fact is that estimates
	 * are computed over much shorter time intervals than the long
	 * intervals typically used for benchmarking. Why? First, to
	 * adapt more quickly to variations. Second, because an I/O
	 * scheduler cannot rely on a peak-rate-evaluation workload to
	 * be run for a long time.
	 */
	ref_wr_duration[0] = msecs_to_jiffies(7000); /* actually 8 sec */
	ref_wr_duration[1] = msecs_to_jiffies(2500); /* actually 3 sec */

	ret = elv_register(&iosched_bfq_mq);
	if (ret)
		goto slab_kill;

	return 0;

slab_kill:
	bfq_slab_kill();
err_pol_unreg:
#ifdef CONFIG_BFQ_GROUP_IOSCHED
	blkcg_policy_unregister(&blkcg_policy_bfq);
#endif
	return ret;
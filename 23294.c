static int __init netif_init(void)
{
	if (!xen_domain())
		return -ENODEV;

	if (!xen_has_pv_nic_devices())
		return -ENODEV;

	pr_info("Initialising Xen virtual ethernet driver\n");

	/* Allow as many queues as there are CPUs inut max. 8 if user has not
	 * specified a value.
	 */
	if (xennet_max_queues == 0)
		xennet_max_queues = min_t(unsigned int, MAX_QUEUES_DEFAULT,
					  num_online_cpus());

	return xenbus_register_frontend(&netfront_driver);
}
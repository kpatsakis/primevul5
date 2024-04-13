int __init dlpar_workqueue_init(void)
{
	if (pseries_hp_wq)
		return 0;

	pseries_hp_wq = alloc_workqueue("pseries hotplug workqueue",
			WQ_UNBOUND, 1);

	return pseries_hp_wq ? 0 : -ENOMEM;
}
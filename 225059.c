static void pseries_hp_work_fn(struct work_struct *work)
{
	struct pseries_hp_work *hp_work =
			container_of(work, struct pseries_hp_work, work);

	handle_dlpar_errorlog(hp_work->errlog);

	kfree(hp_work->errlog);
	kfree((void *)work);
}
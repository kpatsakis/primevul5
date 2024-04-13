void queue_hotplug_event(struct pseries_hp_errorlog *hp_errlog)
{
	struct pseries_hp_work *work;
	struct pseries_hp_errorlog *hp_errlog_copy;

	hp_errlog_copy = kmalloc(sizeof(struct pseries_hp_errorlog),
				 GFP_KERNEL);
	memcpy(hp_errlog_copy, hp_errlog, sizeof(struct pseries_hp_errorlog));

	work = kmalloc(sizeof(struct pseries_hp_work), GFP_KERNEL);
	if (work) {
		INIT_WORK((struct work_struct *)work, pseries_hp_work_fn);
		work->errlog = hp_errlog_copy;
		queue_work(pseries_hp_wq, (struct work_struct *)work);
	} else {
		kfree(hp_errlog_copy);
	}
}
static __init int init_trace_selftests(void)
{
	struct trace_selftests *p, *n;
	struct tracer *t, **last;
	int ret;

	selftests_can_run = true;

	mutex_lock(&trace_types_lock);

	if (list_empty(&postponed_selftests))
		goto out;

	pr_info("Running postponed tracer tests:\n");

	list_for_each_entry_safe(p, n, &postponed_selftests, list) {
		ret = run_tracer_selftest(p->type);
		/* If the test fails, then warn and remove from available_tracers */
		if (ret < 0) {
			WARN(1, "tracer: %s failed selftest, disabling\n",
			     p->type->name);
			last = &trace_types;
			for (t = trace_types; t; t = t->next) {
				if (t == p->type) {
					*last = t->next;
					break;
				}
				last = &t->next;
			}
		}
		list_del(&p->list);
		kfree(p);
	}

 out:
	mutex_unlock(&trace_types_lock);

	return 0;
}
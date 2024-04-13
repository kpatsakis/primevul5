int __init register_tracer(struct tracer *type)
{
	struct tracer *t;
	int ret = 0;

	if (!type->name) {
		pr_info("Tracer must have a name\n");
		return -1;
	}

	if (strlen(type->name) >= MAX_TRACER_SIZE) {
		pr_info("Tracer has a name longer than %d\n", MAX_TRACER_SIZE);
		return -1;
	}

	mutex_lock(&trace_types_lock);

	tracing_selftest_running = true;

	for (t = trace_types; t; t = t->next) {
		if (strcmp(type->name, t->name) == 0) {
			/* already found */
			pr_info("Tracer %s already registered\n",
				type->name);
			ret = -1;
			goto out;
		}
	}

	if (!type->set_flag)
		type->set_flag = &dummy_set_flag;
	if (!type->flags) {
		/*allocate a dummy tracer_flags*/
		type->flags = kmalloc(sizeof(*type->flags), GFP_KERNEL);
		if (!type->flags) {
			ret = -ENOMEM;
			goto out;
		}
		type->flags->val = 0;
		type->flags->opts = dummy_tracer_opt;
	} else
		if (!type->flags->opts)
			type->flags->opts = dummy_tracer_opt;

	/* store the tracer for __set_tracer_option */
	type->flags->trace = type;

	ret = run_tracer_selftest(type);
	if (ret < 0)
		goto out;

	type->next = trace_types;
	trace_types = type;
	add_tracer_options(&global_trace, type);

 out:
	tracing_selftest_running = false;
	mutex_unlock(&trace_types_lock);

	if (ret || !default_bootup_tracer)
		goto out_unlock;

	if (strncmp(default_bootup_tracer, type->name, MAX_TRACER_SIZE))
		goto out_unlock;

	printk(KERN_INFO "Starting tracer '%s'\n", type->name);
	/* Do we want this tracer to start on bootup? */
	tracing_set_tracer(&global_trace, type->name);
	default_bootup_tracer = NULL;

	apply_trace_boot_options();

	/* disable other selftests, since this will break it. */
	tracing_selftest_disabled = true;
#ifdef CONFIG_FTRACE_STARTUP_TEST
	printk(KERN_INFO "Disabling FTRACE selftests due to running tracer '%s'\n",
	       type->name);
#endif

 out_unlock:
	return ret;
}
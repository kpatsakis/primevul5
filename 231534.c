static int trace_module_notify(struct notifier_block *self,
			       unsigned long val, void *data)
{
	struct module *mod = data;

	switch (val) {
	case MODULE_STATE_COMING:
		trace_module_add_evals(mod);
		break;
	case MODULE_STATE_GOING:
		trace_module_remove_evals(mod);
		break;
	}

	return 0;
}
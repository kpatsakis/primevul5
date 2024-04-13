static void trace_module_add_evals(struct module *mod)
{
	if (!mod->num_trace_evals)
		return;

	/*
	 * Modules with bad taint do not have events created, do
	 * not bother with enums either.
	 */
	if (trace_module_has_bad_taint(mod))
		return;

	trace_insert_eval_map(mod, mod->trace_evals, mod->num_trace_evals);
}
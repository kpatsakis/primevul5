void trace_printk_init_buffers(void)
{
	if (buffers_allocated)
		return;

	if (alloc_percpu_trace_buffer())
		return;

	/* trace_printk() is for debug use only. Don't use it in production. */

	pr_warn("\n");
	pr_warn("**********************************************************\n");
	pr_warn("**   NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE   **\n");
	pr_warn("**                                                      **\n");
	pr_warn("** trace_printk() being used. Allocating extra memory.  **\n");
	pr_warn("**                                                      **\n");
	pr_warn("** This means that this is a DEBUG kernel and it is     **\n");
	pr_warn("** unsafe for production use.                           **\n");
	pr_warn("**                                                      **\n");
	pr_warn("** If you see this message and you are not debugging    **\n");
	pr_warn("** the kernel, report this immediately to your vendor!  **\n");
	pr_warn("**                                                      **\n");
	pr_warn("**   NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE   **\n");
	pr_warn("**********************************************************\n");

	/* Expand the buffers to set size */
	tracing_update_buffers();

	buffers_allocated = 1;

	/*
	 * trace_printk_init_buffers() can be called by modules.
	 * If that happens, then we need to start cmdline recording
	 * directly here. If the global_trace.buffer is already
	 * allocated here, then this was called by module code.
	 */
	if (global_trace.trace_buffer.buffer)
		tracing_start_cmdline_record();
}
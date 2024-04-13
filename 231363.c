void __init early_trace_init(void)
{
	if (tracepoint_printk) {
		tracepoint_print_iter =
			kmalloc(sizeof(*tracepoint_print_iter), GFP_KERNEL);
		if (WARN_ON(!tracepoint_print_iter))
			tracepoint_printk = 0;
		else
			static_key_enable(&tracepoint_printk_key.key);
	}
	tracer_alloc_buffers();
}
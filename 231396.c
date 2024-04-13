void __trace_stack(struct trace_array *tr, unsigned long flags, int skip,
		   int pc)
{
	struct ring_buffer *buffer = tr->trace_buffer.buffer;

	if (rcu_is_watching()) {
		__ftrace_trace_stack(buffer, flags, skip, pc, NULL);
		return;
	}

	/*
	 * When an NMI triggers, RCU is enabled via rcu_nmi_enter(),
	 * but if the above rcu_is_watching() failed, then the NMI
	 * triggered someplace critical, and rcu_irq_enter() should
	 * not be called from NMI.
	 */
	if (unlikely(in_nmi()))
		return;

	/*
	 * It is possible that a function is being traced in a
	 * location that RCU is not watching. A call to
	 * rcu_irq_enter() will make sure that it is, but there's
	 * a few internal rcu functions that could be traced
	 * where that wont work either. In those cases, we just
	 * do nothing.
	 */
	if (unlikely(rcu_irq_enter_disabled()))
		return;

	rcu_irq_enter_irqson();
	__ftrace_trace_stack(buffer, flags, skip, pc, NULL);
	rcu_irq_exit_irqson();
}
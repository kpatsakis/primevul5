static void test_cpu_buff_start(struct trace_iterator *iter)
{
	struct trace_seq *s = &iter->seq;
	struct trace_array *tr = iter->tr;

	if (!(tr->trace_flags & TRACE_ITER_ANNOTATE))
		return;

	if (!(iter->iter_flags & TRACE_FILE_ANNOTATE))
		return;

	if (cpumask_available(iter->started) &&
	    cpumask_test_cpu(iter->cpu, iter->started))
		return;

	if (per_cpu_ptr(iter->trace_buffer->data, iter->cpu)->skipped_entries)
		return;

	if (cpumask_available(iter->started))
		cpumask_set_cpu(iter->cpu, iter->started);

	/* Don't print started cpu buffer for the first entry of the trace */
	if (iter->idx > 1)
		trace_seq_printf(s, "##### CPU %u buffer started ####\n",
				iter->cpu);
}
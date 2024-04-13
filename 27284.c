static void *etm_setup_aux(int event_cpu, void **pages,
			   int nr_pages, bool overwrite)
{
	int cpu;
	cpumask_t *mask;
	struct coresight_device *sink;
	struct etm_event_data *event_data = NULL;

	event_data = alloc_event_data(event_cpu);
	if (!event_data)
		return NULL;

	/*
	 * In theory nothing prevent tracers in a trace session from being
	 * associated with different sinks, nor having a sink per tracer.  But
	 * until we have HW with this kind of topology we need to assume tracers
	 * in a trace session are using the same sink.  Therefore go through
	 * the coresight bus and pick the first enabled sink.
	 *
	 * When operated from sysFS users are responsible to enable the sink
	 * while from perf, the perf tools will do it based on the choice made
	 * on the cmd line.  As such the "enable_sink" flag in sysFS is reset.
	 */
	sink = coresight_get_enabled_sink(true);
	if (!sink)
		goto err;

	INIT_WORK(&event_data->work, free_event_data);

	mask = &event_data->mask;

	/* Setup the path for each CPU in a trace session */
	for_each_cpu(cpu, mask) {
		struct coresight_device *csdev;

		csdev = per_cpu(csdev_src, cpu);
		if (!csdev)
			goto err;

		/*
		 * Building a path doesn't enable it, it simply builds a
		 * list of devices from source to sink that can be
		 * referenced later when the path is actually needed.
		 */
		event_data->path[cpu] = coresight_build_path(csdev, sink);
		if (IS_ERR(event_data->path[cpu]))
			goto err;
	}

 	if (!sink_ops(sink)->alloc_buffer)
 		goto err;
 
 	/* Get the AUX specific data from the sink buffer */
 	event_data->snk_config =
 			sink_ops(sink)->alloc_buffer(sink, cpu, pages,
						     nr_pages, overwrite);
	if (!event_data->snk_config)
		goto err;

out:
	return event_data;

err:
	etm_free_aux(event_data);
	event_data = NULL;
	goto out;
}

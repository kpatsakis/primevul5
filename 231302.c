static __init void create_trace_instances(struct dentry *d_tracer)
{
	trace_instance_dir = tracefs_create_instance_dir("instances", d_tracer,
							 instance_mkdir,
							 instance_rmdir);
	if (WARN_ON(!trace_instance_dir))
		return;
}
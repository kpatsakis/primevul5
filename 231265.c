static struct dentry *tracing_get_dentry(struct trace_array *tr)
{
	if (WARN_ON(!tr->dir))
		return ERR_PTR(-ENODEV);

	/* Top directory uses NULL as the parent */
	if (tr->flags & TRACE_ARRAY_FL_GLOBAL)
		return NULL;

	/* All sub buffers have a descriptor */
	return tr->dir;
}
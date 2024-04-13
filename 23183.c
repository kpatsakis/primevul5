static void *cgroup_pidlist_next(struct seq_file *s, void *v, loff_t *pos)
{
	struct kernfs_open_file *of = s->private;
	struct cgroup_file_ctx *ctx = of->priv;
	struct cgroup_pidlist *l = ctx->procs1.pidlist;
	pid_t *p = v;
	pid_t *end = l->list + l->length;
	/*
	 * Advance to the next pid in the array. If this goes off the
	 * end, we're done
	 */
	p++;
	if (p >= end) {
		(*pos)++;
		return NULL;
	} else {
		*pos = *p;
		return p;
	}
}
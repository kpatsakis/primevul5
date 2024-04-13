queue_pages_range(struct mm_struct *mm, unsigned long start, unsigned long end,
		nodemask_t *nodes, unsigned long flags,
		struct list_head *pagelist)
{
	int err;
	struct queue_pages qp = {
		.pagelist = pagelist,
		.flags = flags,
		.nmask = nodes,
		.start = start,
		.end = end,
		.first = NULL,
	};

	err = walk_page_range(mm, start, end, &queue_pages_walk_ops, &qp);

	if (!qp.first)
		/* whole range in hole */
		err = -EFAULT;

	return err;
}
static unsigned interleave_nodes(struct mempolicy *policy)
{
	unsigned next;
	struct task_struct *me = current;

	next = next_node_in(me->il_prev, policy->v.nodes);
	if (next < MAX_NUMNODES)
		me->il_prev = next;
	return next;
}
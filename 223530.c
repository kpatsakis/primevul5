static unsigned offset_il_node(struct mempolicy *pol, unsigned long n)
{
	unsigned nnodes = nodes_weight(pol->v.nodes);
	unsigned target;
	int i;
	int nid;

	if (!nnodes)
		return numa_node_id();
	target = (unsigned int)n % nnodes;
	nid = first_node(pol->v.nodes);
	for (i = 0; i < target; i++)
		nid = next_node(nid, pol->v.nodes);
	return nid;
}
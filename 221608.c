
static void io_rsrc_node_destroy(struct io_rsrc_node *ref_node)
{
	percpu_ref_exit(&ref_node->refs);
	kfree(ref_node);
static void tipc_node_kref_release(struct kref *kref)
{
	struct tipc_node *n = container_of(kref, struct tipc_node, kref);

	kfree(n->bc_entry.link);
	call_rcu(&n->rcu, tipc_node_free);
}
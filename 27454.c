static __net_init int setup_net(struct net *net, struct user_namespace *user_ns)
{
	/* Must be called with pernet_ops_rwsem held */
	const struct pernet_operations *ops, *saved_ops;
	int error = 0;
	LIST_HEAD(net_exit_list);
 
 	refcount_set(&net->count, 1);
 	refcount_set(&net->passive, 1);
 	net->dev_base_seq = 1;
 	net->user_ns = user_ns;
 	idr_init(&net->netns_ids);
	spin_lock_init(&net->nsid_lock);
	mutex_init(&net->ipv4.ra_mutex);

	list_for_each_entry(ops, &pernet_list, list) {
		error = ops_init(ops, net);
		if (error < 0)
			goto out_undo;
	}
	down_write(&net_rwsem);
	list_add_tail_rcu(&net->list, &net_namespace_list);
	up_write(&net_rwsem);
out:
	return error;

out_undo:
	/* Walk through the list backwards calling the exit functions
	 * for the pernet modules whose init functions did not fail.
	 */
	list_add(&net->exit_list, &net_exit_list);
	saved_ops = ops;
	list_for_each_entry_continue_reverse(ops, &pernet_list, list)
		ops_exit_list(ops, &net_exit_list);

	ops = saved_ops;
	list_for_each_entry_continue_reverse(ops, &pernet_list, list)
		ops_free_list(ops, &net_exit_list);

	rcu_barrier();
	goto out;
}

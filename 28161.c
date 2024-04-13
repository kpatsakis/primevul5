int fib6_add(struct fib6_node *root, struct rt6_info *rt, struct nl_info *info)
{
	struct fib6_node *fn, *pn = NULL;
	int err = -ENOMEM;
	int allow_create = 1;
	int replace_required = 0;

	if (info->nlh) {
		if (!(info->nlh->nlmsg_flags & NLM_F_CREATE))
			allow_create = 0;
		if (info->nlh->nlmsg_flags & NLM_F_REPLACE)
			replace_required = 1;
	}
	if (!allow_create && !replace_required)
		pr_warn("RTM_NEWROUTE with no NLM_F_CREATE or NLM_F_REPLACE\n");

 	fn = fib6_add_1(root, &rt->rt6i_dst.addr, rt->rt6i_dst.plen,
 			offsetof(struct rt6_info, rt6i_dst), allow_create,
 			replace_required);
 	if (IS_ERR(fn)) {
 		err = PTR_ERR(fn);
 		goto out;
 	}
 
	pn = fn;

#ifdef CONFIG_IPV6_SUBTREES
	if (rt->rt6i_src.plen) {
		struct fib6_node *sn;

		if (!fn->subtree) {
			struct fib6_node *sfn;

			/*
			 * Create subtree.
			 *
			 *		fn[main tree]
			 *		|
			 *		sfn[subtree root]
			 *		   \
			 *		    sn[new leaf node]
			 */

			/* Create subtree root node */
			sfn = node_alloc();
			if (!sfn)
				goto st_failure;

			sfn->leaf = info->nl_net->ipv6.ip6_null_entry;
			atomic_inc(&info->nl_net->ipv6.ip6_null_entry->rt6i_ref);
			sfn->fn_flags = RTN_ROOT;
			sfn->fn_sernum = fib6_new_sernum();

			/* Now add the first leaf node to new subtree */

			sn = fib6_add_1(sfn, &rt->rt6i_src.addr,
					rt->rt6i_src.plen,
					offsetof(struct rt6_info, rt6i_src),
					allow_create, replace_required);

			if (IS_ERR(sn)) {
				/* If it is failed, discard just allocated
				   root, and then (in st_failure) stale node
				   in main tree.
				 */
				node_free(sfn);
				err = PTR_ERR(sn);
				goto st_failure;
			}

			/* Now link new subtree to main tree */
			sfn->parent = fn;
			fn->subtree = sfn;
		} else {
			sn = fib6_add_1(fn->subtree, &rt->rt6i_src.addr,
					rt->rt6i_src.plen,
					offsetof(struct rt6_info, rt6i_src),
					allow_create, replace_required);

			if (IS_ERR(sn)) {
				err = PTR_ERR(sn);
				goto st_failure;
			}
		}

		if (!fn->leaf) {
			fn->leaf = rt;
			atomic_inc(&rt->rt6i_ref);
		}
		fn = sn;
	}
#endif

	err = fib6_add_rt2node(fn, rt, info);
	if (!err) {
		fib6_start_gc(info->nl_net, rt);
		if (!(rt->rt6i_flags & RTF_CACHE))
			fib6_prune_clones(info->nl_net, pn, rt);
	}

out:
	if (err) {
#ifdef CONFIG_IPV6_SUBTREES
		/*
		 * If fib6_add_1 has cleared the old leaf pointer in the
		 * super-tree leaf node we have to find a new one for it.
		 */
		if (pn != fn && pn->leaf == rt) {
			pn->leaf = NULL;
			atomic_dec(&rt->rt6i_ref);
		}
		if (pn != fn && !pn->leaf && !(pn->fn_flags & RTN_RTINFO)) {
			pn->leaf = fib6_find_prefix(info->nl_net, pn);
#if RT6_DEBUG >= 2
			if (!pn->leaf) {
				WARN_ON(pn->leaf == NULL);
				pn->leaf = info->nl_net->ipv6.ip6_null_entry;
			}
#endif
			atomic_inc(&pn->leaf->rt6i_ref);
		}
#endif
		dst_free(&rt->dst);
	}
	return err;

#ifdef CONFIG_IPV6_SUBTREES
	/* Subtree creation failed, probably main tree node
	   is orphan. If it is, shoot it.
	 */
st_failure:
	if (fn && !(fn->fn_flags & (RTN_RTINFO|RTN_ROOT)))
		fib6_repair_tree(info->nl_net, fn);
	dst_free(&rt->dst);
	return err;
#endif
}

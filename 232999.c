static int ip6_dst_lookup_tail(struct net *net, const struct sock *sk,
			       struct dst_entry **dst, struct flowi6 *fl6)
{
#ifdef CONFIG_IPV6_OPTIMISTIC_DAD
	struct neighbour *n;
	struct rt6_info *rt;
#endif
	int err;
	int flags = 0;

	/* The correct way to handle this would be to do
	 * ip6_route_get_saddr, and then ip6_route_output; however,
	 * the route-specific preferred source forces the
	 * ip6_route_output call _before_ ip6_route_get_saddr.
	 *
	 * In source specific routing (no src=any default route),
	 * ip6_route_output will fail given src=any saddr, though, so
	 * that's why we try it again later.
	 */
	if (ipv6_addr_any(&fl6->saddr) && (!*dst || !(*dst)->error)) {
		struct rt6_info *rt;
		bool had_dst = *dst != NULL;

		if (!had_dst)
			*dst = ip6_route_output(net, sk, fl6);
		rt = (*dst)->error ? NULL : (struct rt6_info *)*dst;
		err = ip6_route_get_saddr(net, rt, &fl6->daddr,
					  sk ? inet6_sk(sk)->srcprefs : 0,
					  &fl6->saddr);
		if (err)
			goto out_err_release;

		/* If we had an erroneous initial result, pretend it
		 * never existed and let the SA-enabled version take
		 * over.
		 */
		if (!had_dst && (*dst)->error) {
			dst_release(*dst);
			*dst = NULL;
		}

		if (fl6->flowi6_oif)
			flags |= RT6_LOOKUP_F_IFACE;
	}

	if (!*dst)
		*dst = ip6_route_output_flags(net, sk, fl6, flags);

	err = (*dst)->error;
	if (err)
		goto out_err_release;

#ifdef CONFIG_IPV6_OPTIMISTIC_DAD
	/*
	 * Here if the dst entry we've looked up
	 * has a neighbour entry that is in the INCOMPLETE
	 * state and the src address from the flow is
	 * marked as OPTIMISTIC, we release the found
	 * dst entry and replace it instead with the
	 * dst entry of the nexthop router
	 */
	rt = (struct rt6_info *) *dst;
	rcu_read_lock_bh();
	n = __ipv6_neigh_lookup_noref(rt->dst.dev,
				      rt6_nexthop(rt, &fl6->daddr));
	err = n && !(n->nud_state & NUD_VALID) ? -EINVAL : 0;
	rcu_read_unlock_bh();

	if (err) {
		struct inet6_ifaddr *ifp;
		struct flowi6 fl_gw6;
		int redirect;

		ifp = ipv6_get_ifaddr(net, &fl6->saddr,
				      (*dst)->dev, 1);

		redirect = (ifp && ifp->flags & IFA_F_OPTIMISTIC);
		if (ifp)
			in6_ifa_put(ifp);

		if (redirect) {
			/*
			 * We need to get the dst entry for the
			 * default router instead
			 */
			dst_release(*dst);
			memcpy(&fl_gw6, fl6, sizeof(struct flowi6));
			memset(&fl_gw6.daddr, 0, sizeof(struct in6_addr));
			*dst = ip6_route_output(net, sk, &fl_gw6);
			err = (*dst)->error;
			if (err)
				goto out_err_release;
		}
	}
#endif
	if (ipv6_addr_v4mapped(&fl6->saddr) &&
	    !(ipv6_addr_v4mapped(&fl6->daddr) || ipv6_addr_any(&fl6->daddr))) {
		err = -EAFNOSUPPORT;
		goto out_err_release;
	}

	return 0;

out_err_release:
	dst_release(*dst);
	*dst = NULL;

	if (err == -ENETUNREACH)
		IP6_INC_STATS(net, NULL, IPSTATS_MIB_OUTNOROUTES);
	return err;
}
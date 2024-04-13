static void icmp6_send(struct sk_buff *skb, u8 type, u8 code, __u32 info,
		       const struct in6_addr *force_saddr)
{
	struct net *net = dev_net(skb->dev);
	struct inet6_dev *idev = NULL;
	struct ipv6hdr *hdr = ipv6_hdr(skb);
	struct sock *sk;
	struct ipv6_pinfo *np;
	const struct in6_addr *saddr = NULL;
	struct dst_entry *dst;
	struct icmp6hdr tmp_hdr;
	struct flowi6 fl6;
	struct icmpv6_msg msg;
	struct sockcm_cookie sockc_unused = {0};
	struct ipcm6_cookie ipc6;
	int iif = 0;
	int addr_type = 0;
	int len;
	int err = 0;
	u32 mark = IP6_REPLY_MARK(net, skb->mark);

	if ((u8 *)hdr < skb->head ||
	    (skb_network_header(skb) + sizeof(*hdr)) > skb_tail_pointer(skb))
		return;

	/*
	 *	Make sure we respect the rules
	 *	i.e. RFC 1885 2.4(e)
	 *	Rule (e.1) is enforced by not using icmp6_send
	 *	in any code that processes icmp errors.
	 */
	addr_type = ipv6_addr_type(&hdr->daddr);

	if (ipv6_chk_addr(net, &hdr->daddr, skb->dev, 0) ||
	    ipv6_chk_acast_addr_src(net, skb->dev, &hdr->daddr))
		saddr = &hdr->daddr;

	/*
	 *	Dest addr check
	 */

	if (addr_type & IPV6_ADDR_MULTICAST || skb->pkt_type != PACKET_HOST) {
		if (type != ICMPV6_PKT_TOOBIG &&
		    !(type == ICMPV6_PARAMPROB &&
		      code == ICMPV6_UNK_OPTION &&
		      (opt_unrec(skb, info))))
			return;

		saddr = NULL;
	}

	addr_type = ipv6_addr_type(&hdr->saddr);

	/*
	 *	Source addr check
	 */
 
 	if (__ipv6_addr_needs_scope_id(addr_type))
 		iif = skb->dev->ifindex;
	else
		iif = l3mdev_master_ifindex(skb_dst(skb)->dev);
 
 	/*
 	 *	Must not send error if the source does not uniquely
	 *	identify a single node (RFC2463 Section 2.4).
	 *	We check unspecified / multicast addresses here,
	 *	and anycast addresses will be checked later.
	 */
	if ((addr_type == IPV6_ADDR_ANY) || (addr_type & IPV6_ADDR_MULTICAST)) {
		net_dbg_ratelimited("icmp6_send: addr_any/mcast source [%pI6c > %pI6c]\n",
				    &hdr->saddr, &hdr->daddr);
		return;
	}

	/*
	 *	Never answer to a ICMP packet.
	 */
	if (is_ineligible(skb)) {
		net_dbg_ratelimited("icmp6_send: no reply to icmp error [%pI6c > %pI6c]\n",
				    &hdr->saddr, &hdr->daddr);
		return;
	}

	mip6_addr_swap(skb);

	memset(&fl6, 0, sizeof(fl6));
	fl6.flowi6_proto = IPPROTO_ICMPV6;
	fl6.daddr = hdr->saddr;
	if (force_saddr)
		saddr = force_saddr;
	if (saddr)
		fl6.saddr = *saddr;
	fl6.flowi6_mark = mark;
	fl6.flowi6_oif = iif;
	fl6.fl6_icmp_type = type;
	fl6.fl6_icmp_code = code;
	security_skb_classify_flow(skb, flowi6_to_flowi(&fl6));

	sk = icmpv6_xmit_lock(net);
	if (!sk)
		return;
	sk->sk_mark = mark;
	np = inet6_sk(sk);

	if (!icmpv6_xrlim_allow(sk, type, &fl6))
		goto out;

	tmp_hdr.icmp6_type = type;
	tmp_hdr.icmp6_code = code;
	tmp_hdr.icmp6_cksum = 0;
	tmp_hdr.icmp6_pointer = htonl(info);

	if (!fl6.flowi6_oif && ipv6_addr_is_multicast(&fl6.daddr))
		fl6.flowi6_oif = np->mcast_oif;
	else if (!fl6.flowi6_oif)
		fl6.flowi6_oif = np->ucast_oif;

	ipc6.tclass = np->tclass;
	fl6.flowlabel = ip6_make_flowinfo(ipc6.tclass, fl6.flowlabel);

	dst = icmpv6_route_lookup(net, skb, sk, &fl6);
	if (IS_ERR(dst))
		goto out;

	ipc6.hlimit = ip6_sk_dst_hoplimit(np, &fl6, dst);
	ipc6.dontfrag = np->dontfrag;
	ipc6.opt = NULL;

	msg.skb = skb;
	msg.offset = skb_network_offset(skb);
	msg.type = type;

	len = skb->len - msg.offset;
	len = min_t(unsigned int, len, IPV6_MIN_MTU - sizeof(struct ipv6hdr) - sizeof(struct icmp6hdr));
	if (len < 0) {
		net_dbg_ratelimited("icmp: len problem [%pI6c > %pI6c]\n",
				    &hdr->saddr, &hdr->daddr);
		goto out_dst_release;
	}

	rcu_read_lock();
	idev = __in6_dev_get(skb->dev);

	err = ip6_append_data(sk, icmpv6_getfrag, &msg,
			      len + sizeof(struct icmp6hdr),
			      sizeof(struct icmp6hdr),
			      &ipc6, &fl6, (struct rt6_info *)dst,
			      MSG_DONTWAIT, &sockc_unused);
	if (err) {
		ICMP6_INC_STATS(net, idev, ICMP6_MIB_OUTERRORS);
		ip6_flush_pending_frames(sk);
	} else {
		err = icmpv6_push_pending_frames(sk, &fl6, &tmp_hdr,
						 len + sizeof(struct icmp6hdr));
	}
	rcu_read_unlock();
out_dst_release:
	dst_release(dst);
out:
	icmpv6_xmit_unlock(sk);
}

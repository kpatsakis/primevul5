void tipc_node_check_dest(struct net *net, u32 addr,
			  u8 *peer_id, struct tipc_bearer *b,
			  u16 capabilities, u32 signature, u32 hash_mixes,
			  struct tipc_media_addr *maddr,
			  bool *respond, bool *dupl_addr)
{
	struct tipc_node *n;
	struct tipc_link *l, *snd_l;
	struct tipc_link_entry *le;
	bool addr_match = false;
	bool sign_match = false;
	bool link_up = false;
	bool accept_addr = false;
	bool reset = true;
	char *if_name;
	unsigned long intv;
	u16 session;

	*dupl_addr = false;
	*respond = false;

	n = tipc_node_create(net, addr, peer_id, capabilities, hash_mixes,
			     false);
	if (!n)
		return;

	tipc_node_write_lock(n);
	if (unlikely(!n->bc_entry.link)) {
		snd_l = tipc_bc_sndlink(net);
		if (!tipc_link_bc_create(net, tipc_own_addr(net),
					 addr, peer_id, U16_MAX,
					 tipc_link_min_win(snd_l),
					 tipc_link_max_win(snd_l),
					 n->capabilities,
					 &n->bc_entry.inputq1,
					 &n->bc_entry.namedq, snd_l,
					 &n->bc_entry.link)) {
			pr_warn("Broadcast rcv link creation failed, no mem\n");
			tipc_node_write_unlock_fast(n);
			tipc_node_put(n);
			return;
		}
	}

	le = &n->links[b->identity];

	/* Prepare to validate requesting node's signature and media address */
	l = le->link;
	link_up = l && tipc_link_is_up(l);
	addr_match = l && !memcmp(&le->maddr, maddr, sizeof(*maddr));
	sign_match = (signature == n->signature);

	/* These three flags give us eight permutations: */

	if (sign_match && addr_match && link_up) {
		/* All is fine. Do nothing. */
		reset = false;
		/* Peer node is not a container/local namespace */
		if (!n->peer_hash_mix)
			n->peer_hash_mix = hash_mixes;
	} else if (sign_match && addr_match && !link_up) {
		/* Respond. The link will come up in due time */
		*respond = true;
	} else if (sign_match && !addr_match && link_up) {
		/* Peer has changed i/f address without rebooting.
		 * If so, the link will reset soon, and the next
		 * discovery will be accepted. So we can ignore it.
		 * It may also be an cloned or malicious peer having
		 * chosen the same node address and signature as an
		 * existing one.
		 * Ignore requests until the link goes down, if ever.
		 */
		*dupl_addr = true;
	} else if (sign_match && !addr_match && !link_up) {
		/* Peer link has changed i/f address without rebooting.
		 * It may also be a cloned or malicious peer; we can't
		 * distinguish between the two.
		 * The signature is correct, so we must accept.
		 */
		accept_addr = true;
		*respond = true;
	} else if (!sign_match && addr_match && link_up) {
		/* Peer node rebooted. Two possibilities:
		 *  - Delayed re-discovery; this link endpoint has already
		 *    reset and re-established contact with the peer, before
		 *    receiving a discovery message from that node.
		 *    (The peer happened to receive one from this node first).
		 *  - The peer came back so fast that our side has not
		 *    discovered it yet. Probing from this side will soon
		 *    reset the link, since there can be no working link
		 *    endpoint at the peer end, and the link will re-establish.
		 *  Accept the signature, since it comes from a known peer.
		 */
		n->signature = signature;
	} else if (!sign_match && addr_match && !link_up) {
		/*  The peer node has rebooted.
		 *  Accept signature, since it is a known peer.
		 */
		n->signature = signature;
		*respond = true;
	} else if (!sign_match && !addr_match && link_up) {
		/* Peer rebooted with new address, or a new/duplicate peer.
		 * Ignore until the link goes down, if ever.
		 */
		*dupl_addr = true;
	} else if (!sign_match && !addr_match && !link_up) {
		/* Peer rebooted with new address, or it is a new peer.
		 * Accept signature and address.
		 */
		n->signature = signature;
		accept_addr = true;
		*respond = true;
	}

	if (!accept_addr)
		goto exit;

	/* Now create new link if not already existing */
	if (!l) {
		if (n->link_cnt == 2)
			goto exit;

		if_name = strchr(b->name, ':') + 1;
		get_random_bytes(&session, sizeof(u16));
		if (!tipc_link_create(net, if_name, b->identity, b->tolerance,
				      b->net_plane, b->mtu, b->priority,
				      b->min_win, b->max_win, session,
				      tipc_own_addr(net), addr, peer_id,
				      n->capabilities,
				      tipc_bc_sndlink(n->net), n->bc_entry.link,
				      &le->inputq,
				      &n->bc_entry.namedq, &l)) {
			*respond = false;
			goto exit;
		}
		trace_tipc_link_reset(l, TIPC_DUMP_ALL, "link created!");
		tipc_link_reset(l);
		tipc_link_fsm_evt(l, LINK_RESET_EVT);
		if (n->state == NODE_FAILINGOVER)
			tipc_link_fsm_evt(l, LINK_FAILOVER_BEGIN_EVT);
		le->link = l;
		n->link_cnt++;
		tipc_node_calculate_timer(n, l);
		if (n->link_cnt == 1) {
			intv = jiffies + msecs_to_jiffies(n->keepalive_intv);
			if (!mod_timer(&n->timer, intv))
				tipc_node_get(n);
		}
	}
	memcpy(&le->maddr, maddr, sizeof(*maddr));
exit:
	tipc_node_write_unlock(n);
	if (reset && l && !tipc_link_is_reset(l))
		tipc_node_link_down(n, b->identity, false);
	tipc_node_put(n);
}
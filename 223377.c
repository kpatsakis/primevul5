void tipc_rcv(struct net *net, struct sk_buff *skb, struct tipc_bearer *b)
{
	struct sk_buff_head xmitq;
	struct tipc_link_entry *le;
	struct tipc_msg *hdr;
	struct tipc_node *n;
	int bearer_id = b->identity;
	u32 self = tipc_own_addr(net);
	int usr, rc = 0;
	u16 bc_ack;
#ifdef CONFIG_TIPC_CRYPTO
	struct tipc_ehdr *ehdr;

	/* Check if message must be decrypted first */
	if (TIPC_SKB_CB(skb)->decrypted || !tipc_ehdr_validate(skb))
		goto rcv;

	ehdr = (struct tipc_ehdr *)skb->data;
	if (likely(ehdr->user != LINK_CONFIG)) {
		n = tipc_node_find(net, ntohl(ehdr->addr));
		if (unlikely(!n))
			goto discard;
	} else {
		n = tipc_node_find_by_id(net, ehdr->id);
	}
	tipc_crypto_rcv(net, (n) ? n->crypto_rx : NULL, &skb, b);
	if (!skb)
		return;

rcv:
#endif
	/* Ensure message is well-formed before touching the header */
	if (unlikely(!tipc_msg_validate(&skb)))
		goto discard;
	__skb_queue_head_init(&xmitq);
	hdr = buf_msg(skb);
	usr = msg_user(hdr);
	bc_ack = msg_bcast_ack(hdr);

	/* Handle arrival of discovery or broadcast packet */
	if (unlikely(msg_non_seq(hdr))) {
		if (unlikely(usr == LINK_CONFIG))
			return tipc_disc_rcv(net, skb, b);
		else
			return tipc_node_bc_rcv(net, skb, bearer_id);
	}

	/* Discard unicast link messages destined for another node */
	if (unlikely(!msg_short(hdr) && (msg_destnode(hdr) != self)))
		goto discard;

	/* Locate neighboring node that sent packet */
	n = tipc_node_find(net, msg_prevnode(hdr));
	if (unlikely(!n))
		goto discard;
	le = &n->links[bearer_id];

	/* Ensure broadcast reception is in synch with peer's send state */
	if (unlikely(usr == LINK_PROTOCOL)) {
		if (unlikely(skb_linearize(skb))) {
			tipc_node_put(n);
			goto discard;
		}
		hdr = buf_msg(skb);
		tipc_node_bc_sync_rcv(n, hdr, bearer_id, &xmitq);
	} else if (unlikely(tipc_link_acked(n->bc_entry.link) != bc_ack)) {
		tipc_bcast_ack_rcv(net, n->bc_entry.link, hdr);
	}

	/* Receive packet directly if conditions permit */
	tipc_node_read_lock(n);
	if (likely((n->state == SELF_UP_PEER_UP) && (usr != TUNNEL_PROTOCOL))) {
		spin_lock_bh(&le->lock);
		if (le->link) {
			rc = tipc_link_rcv(le->link, skb, &xmitq);
			skb = NULL;
		}
		spin_unlock_bh(&le->lock);
	}
	tipc_node_read_unlock(n);

	/* Check/update node state before receiving */
	if (unlikely(skb)) {
		if (unlikely(skb_linearize(skb)))
			goto out_node_put;
		tipc_node_write_lock(n);
		if (tipc_node_check_state(n, skb, bearer_id, &xmitq)) {
			if (le->link) {
				rc = tipc_link_rcv(le->link, skb, &xmitq);
				skb = NULL;
			}
		}
		tipc_node_write_unlock(n);
	}

	if (unlikely(rc & TIPC_LINK_UP_EVT))
		tipc_node_link_up(n, bearer_id, &xmitq);

	if (unlikely(rc & TIPC_LINK_DOWN_EVT))
		tipc_node_link_down(n, bearer_id, false);

	if (unlikely(!skb_queue_empty(&n->bc_entry.namedq)))
		tipc_named_rcv(net, &n->bc_entry.namedq,
			       &n->bc_entry.named_rcv_nxt,
			       &n->bc_entry.named_open);

	if (unlikely(!skb_queue_empty(&n->bc_entry.inputq1)))
		tipc_node_mcast_rcv(n);

	if (!skb_queue_empty(&le->inputq))
		tipc_sk_rcv(net, &le->inputq);

	if (!skb_queue_empty(&xmitq))
		tipc_bearer_xmit(net, bearer_id, &xmitq, &le->maddr, n);

out_node_put:
	tipc_node_put(n);
discard:
	kfree_skb(skb);
}
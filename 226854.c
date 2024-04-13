static int sctp_sock_migrate(struct sock *oldsk, struct sock *newsk,
			     struct sctp_association *assoc,
			     enum sctp_socket_type type)
{
	struct sctp_sock *oldsp = sctp_sk(oldsk);
	struct sctp_sock *newsp = sctp_sk(newsk);
	struct sctp_bind_bucket *pp; /* hash list port iterator */
	struct sctp_endpoint *newep = newsp->ep;
	struct sk_buff *skb, *tmp;
	struct sctp_ulpevent *event;
	struct sctp_bind_hashbucket *head;
	int err;

	/* Migrate socket buffer sizes and all the socket level options to the
	 * new socket.
	 */
	newsk->sk_sndbuf = oldsk->sk_sndbuf;
	newsk->sk_rcvbuf = oldsk->sk_rcvbuf;
	/* Brute force copy old sctp opt. */
	sctp_copy_descendant(newsk, oldsk);

	/* Restore the ep value that was overwritten with the above structure
	 * copy.
	 */
	newsp->ep = newep;
	newsp->hmac = NULL;

	/* Hook this new socket in to the bind_hash list. */
	head = &sctp_port_hashtable[sctp_phashfn(sock_net(oldsk),
						 inet_sk(oldsk)->inet_num)];
	spin_lock_bh(&head->lock);
	pp = sctp_sk(oldsk)->bind_hash;
	sk_add_bind_node(newsk, &pp->owner);
	sctp_sk(newsk)->bind_hash = pp;
	inet_sk(newsk)->inet_num = inet_sk(oldsk)->inet_num;
	spin_unlock_bh(&head->lock);

	/* Copy the bind_addr list from the original endpoint to the new
	 * endpoint so that we can handle restarts properly
	 */
	err = sctp_bind_addr_dup(&newsp->ep->base.bind_addr,
				 &oldsp->ep->base.bind_addr, GFP_KERNEL);
	if (err)
		return err;

	/* New ep's auth_hmacs should be set if old ep's is set, in case
	 * that net->sctp.auth_enable has been changed to 0 by users and
	 * new ep's auth_hmacs couldn't be set in sctp_endpoint_init().
	 */
	if (oldsp->ep->auth_hmacs) {
		err = sctp_auth_init_hmacs(newsp->ep, GFP_KERNEL);
		if (err)
			return err;
	}

	/* Move any messages in the old socket's receive queue that are for the
	 * peeled off association to the new socket's receive queue.
	 */
	sctp_skb_for_each(skb, &oldsk->sk_receive_queue, tmp) {
		event = sctp_skb2event(skb);
		if (event->asoc == assoc) {
			__skb_unlink(skb, &oldsk->sk_receive_queue);
			__skb_queue_tail(&newsk->sk_receive_queue, skb);
			sctp_skb_set_owner_r_frag(skb, newsk);
		}
	}

	/* Clean up any messages pending delivery due to partial
	 * delivery.   Three cases:
	 * 1) No partial deliver;  no work.
	 * 2) Peeling off partial delivery; keep pd_lobby in new pd_lobby.
	 * 3) Peeling off non-partial delivery; move pd_lobby to receive_queue.
	 */
	atomic_set(&sctp_sk(newsk)->pd_mode, assoc->ulpq.pd_mode);

	if (atomic_read(&sctp_sk(oldsk)->pd_mode)) {
		struct sk_buff_head *queue;

		/* Decide which queue to move pd_lobby skbs to. */
		if (assoc->ulpq.pd_mode) {
			queue = &newsp->pd_lobby;
		} else
			queue = &newsk->sk_receive_queue;

		/* Walk through the pd_lobby, looking for skbs that
		 * need moved to the new socket.
		 */
		sctp_skb_for_each(skb, &oldsp->pd_lobby, tmp) {
			event = sctp_skb2event(skb);
			if (event->asoc == assoc) {
				__skb_unlink(skb, &oldsp->pd_lobby);
				__skb_queue_tail(queue, skb);
				sctp_skb_set_owner_r_frag(skb, newsk);
			}
		}

		/* Clear up any skbs waiting for the partial
		 * delivery to finish.
		 */
		if (assoc->ulpq.pd_mode)
			sctp_clear_pd(oldsk, NULL);

	}

	sctp_for_each_rx_skb(assoc, newsk, sctp_skb_set_owner_r_frag);

	/* Set the type of socket to indicate that it is peeled off from the
	 * original UDP-style socket or created with the accept() call on a
	 * TCP-style socket..
	 */
	newsp->type = type;

	/* Mark the new socket "in-use" by the user so that any packets
	 * that may arrive on the association after we've moved it are
	 * queued to the backlog.  This prevents a potential race between
	 * backlog processing on the old socket and new-packet processing
	 * on the new socket.
	 *
	 * The caller has just allocated newsk so we can guarantee that other
	 * paths won't try to lock it and then oldsk.
	 */
	lock_sock_nested(newsk, SINGLE_DEPTH_NESTING);
	sctp_for_each_tx_datachunk(assoc, true, sctp_clear_owner_w);
	sctp_assoc_migrate(assoc, newsk);
	sctp_for_each_tx_datachunk(assoc, false, sctp_set_owner_w);

	/* If the association on the newsk is already closed before accept()
	 * is called, set RCV_SHUTDOWN flag.
	 */
	if (sctp_state(assoc, CLOSED) && sctp_style(newsk, TCP)) {
		inet_sk_set_state(newsk, SCTP_SS_CLOSED);
		newsk->sk_shutdown |= RCV_SHUTDOWN;
	} else {
		inet_sk_set_state(newsk, SCTP_SS_ESTABLISHED);
	}

	release_sock(newsk);

	return 0;
}
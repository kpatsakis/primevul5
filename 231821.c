int x25_rx_call_request(struct sk_buff *skb, struct x25_neigh *nb,
			unsigned int lci)
{
	struct sock *sk;
	struct sock *make;
	struct x25_sock *makex25;
	struct x25_address source_addr, dest_addr;
	struct x25_facilities facilities;
	struct x25_dte_facilities dte_facilities;
	int len, addr_len, rc;

	/*
	 *	Remove the LCI and frame type.
	 */
	skb_pull(skb, X25_STD_MIN_LEN);

	/*
	 *	Extract the X.25 addresses and convert them to ASCII strings,
	 *	and remove them.
	 *
	 *	Address block is mandatory in call request packets
	 */
	addr_len = x25_parse_address_block(skb, &source_addr, &dest_addr);
	if (addr_len <= 0)
		goto out_clear_request;
	skb_pull(skb, addr_len);

	/*
	 *	Get the length of the facilities, skip past them for the moment
	 *	get the call user data because this is needed to determine
	 *	the correct listener
	 *
	 *	Facilities length is mandatory in call request packets
	 */
	if (!pskb_may_pull(skb, 1))
		goto out_clear_request;
	len = skb->data[0] + 1;
	if (!pskb_may_pull(skb, len))
		goto out_clear_request;
	skb_pull(skb,len);

	/*
	 *	Ensure that the amount of call user data is valid.
	 */
	if (skb->len > X25_MAX_CUD_LEN)
		goto out_clear_request;

	/*
	 *	Get all the call user data so it can be used in
	 *	x25_find_listener and skb_copy_from_linear_data up ahead.
	 */
	if (!pskb_may_pull(skb, skb->len))
		goto out_clear_request;

	/*
	 *	Find a listener for the particular address/cud pair.
	 */
	sk = x25_find_listener(&source_addr,skb);
	skb_push(skb,len);

	if (sk != NULL && sk_acceptq_is_full(sk)) {
		goto out_sock_put;
	}

	/*
	 *	We dont have any listeners for this incoming call.
	 *	Try forwarding it.
	 */
	if (sk == NULL) {
		skb_push(skb, addr_len + X25_STD_MIN_LEN);
		if (sysctl_x25_forward &&
				x25_forward_call(&dest_addr, nb, skb, lci) > 0)
		{
			/* Call was forwarded, dont process it any more */
			kfree_skb(skb);
			rc = 1;
			goto out;
		} else {
			/* No listeners, can't forward, clear the call */
			goto out_clear_request;
		}
	}

	/*
	 *	Try to reach a compromise on the requested facilities.
	 */
	len = x25_negotiate_facilities(skb, sk, &facilities, &dte_facilities);
	if (len == -1)
		goto out_sock_put;

	/*
	 * current neighbour/link might impose additional limits
	 * on certain facilities
	 */

	x25_limit_facilities(&facilities, nb);

	/*
	 *	Try to create a new socket.
	 */
	make = x25_make_new(sk);
	if (!make)
		goto out_sock_put;

	/*
	 *	Remove the facilities
	 */
	skb_pull(skb, len);

	skb->sk     = make;
	make->sk_state = TCP_ESTABLISHED;

	makex25 = x25_sk(make);
	makex25->lci           = lci;
	makex25->dest_addr     = dest_addr;
	makex25->source_addr   = source_addr;
	x25_neigh_hold(nb);
	makex25->neighbour     = nb;
	makex25->facilities    = facilities;
	makex25->dte_facilities= dte_facilities;
	makex25->vc_facil_mask = x25_sk(sk)->vc_facil_mask;
	/* ensure no reverse facil on accept */
	makex25->vc_facil_mask &= ~X25_MASK_REVERSE;
	/* ensure no calling address extension on accept */
	makex25->vc_facil_mask &= ~X25_MASK_CALLING_AE;
	makex25->cudmatchlength = x25_sk(sk)->cudmatchlength;

	/* Normally all calls are accepted immediately */
	if (test_bit(X25_ACCPT_APPRV_FLAG, &makex25->flags)) {
		x25_write_internal(make, X25_CALL_ACCEPTED);
		makex25->state = X25_STATE_3;
	} else {
		makex25->state = X25_STATE_5;
	}

	/*
	 *	Incoming Call User Data.
	 */
	skb_copy_from_linear_data(skb, makex25->calluserdata.cuddata, skb->len);
	makex25->calluserdata.cudlength = skb->len;

	sk_acceptq_added(sk);

	x25_insert_socket(make);

	skb_queue_head(&sk->sk_receive_queue, skb);

	x25_start_heartbeat(make);

	if (!sock_flag(sk, SOCK_DEAD))
		sk->sk_data_ready(sk);
	rc = 1;
	sock_put(sk);
out:
	return rc;
out_sock_put:
	sock_put(sk);
out_clear_request:
	rc = 0;
	x25_transmit_clear_request(nb, lci, 0x01);
	goto out;
}
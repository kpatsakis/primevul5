static void tcp_data_queue(struct sock *sk, struct sk_buff *skb)
{
	const struct tcphdr *th = tcp_hdr(skb);
	struct tcp_sock *tp = tcp_sk(sk);
	int eaten = -1;

	if (TCP_SKB_CB(skb)->seq == TCP_SKB_CB(skb)->end_seq)
		goto drop;

	skb_dst_drop(skb);
	__skb_pull(skb, th->doff * 4);

	TCP_ECN_accept_cwr(tp, skb);

	tp->rx_opt.dsack = 0;

	/*  Queue data for delivery to the user.
	 *  Packets in sequence go to the receive queue.
	 *  Out of sequence packets to the out_of_order_queue.
	 */
	if (TCP_SKB_CB(skb)->seq == tp->rcv_nxt) {
		if (tcp_receive_window(tp) == 0)
			goto out_of_window;

		/* Ok. In sequence. In window. */
		if (tp->ucopy.task == current &&
		    tp->copied_seq == tp->rcv_nxt && tp->ucopy.len &&
		    sock_owned_by_user(sk) && !tp->urg_data) {
			int chunk = min_t(unsigned int, skb->len,
					  tp->ucopy.len);

			__set_current_state(TASK_RUNNING);

			local_bh_enable();
			if (!skb_copy_datagram_iovec(skb, 0, tp->ucopy.iov, chunk)) {
				tp->ucopy.len -= chunk;
				tp->copied_seq += chunk;
				eaten = (chunk == skb->len);
				tcp_rcv_space_adjust(sk);
			}
			local_bh_disable();
		}

		if (eaten <= 0) {
queue_and_out:
			if (eaten < 0 &&
			    tcp_try_rmem_schedule(sk, skb->truesize))
				goto drop;

			skb_set_owner_r(skb, sk);
			__skb_queue_tail(&sk->sk_receive_queue, skb);
		}
		tp->rcv_nxt = TCP_SKB_CB(skb)->end_seq;
		if (skb->len)
			tcp_event_data_recv(sk, skb);
		if (th->fin)
			tcp_fin(sk);

		if (!skb_queue_empty(&tp->out_of_order_queue)) {
			tcp_ofo_queue(sk);

			/* RFC2581. 4.2. SHOULD send immediate ACK, when
			 * gap in queue is filled.
			 */
			if (skb_queue_empty(&tp->out_of_order_queue))
				inet_csk(sk)->icsk_ack.pingpong = 0;
		}

		if (tp->rx_opt.num_sacks)
			tcp_sack_remove(tp);

		tcp_fast_path_check(sk);

		if (eaten > 0)
			__kfree_skb(skb);
		else if (!sock_flag(sk, SOCK_DEAD))
			sk->sk_data_ready(sk, 0);
		return;
	}

	if (!after(TCP_SKB_CB(skb)->end_seq, tp->rcv_nxt)) {
		/* A retransmit, 2nd most common case.  Force an immediate ack. */
		NET_INC_STATS_BH(sock_net(sk), LINUX_MIB_DELAYEDACKLOST);
		tcp_dsack_set(sk, TCP_SKB_CB(skb)->seq, TCP_SKB_CB(skb)->end_seq);

out_of_window:
		tcp_enter_quickack_mode(sk);
		inet_csk_schedule_ack(sk);
drop:
		__kfree_skb(skb);
		return;
	}

	/* Out of window. F.e. zero window probe. */
	if (!before(TCP_SKB_CB(skb)->seq, tp->rcv_nxt + tcp_receive_window(tp)))
		goto out_of_window;

	tcp_enter_quickack_mode(sk);

	if (before(TCP_SKB_CB(skb)->seq, tp->rcv_nxt)) {
		/* Partial packet, seq < rcv_next < end_seq */
		SOCK_DEBUG(sk, "partial packet: rcv_next %X seq %X - %X\n",
			   tp->rcv_nxt, TCP_SKB_CB(skb)->seq,
			   TCP_SKB_CB(skb)->end_seq);

		tcp_dsack_set(sk, TCP_SKB_CB(skb)->seq, tp->rcv_nxt);

		/* If window is closed, drop tail of packet. But after
		 * remembering D-SACK for its head made in previous line.
		 */
		if (!tcp_receive_window(tp))
			goto out_of_window;
		goto queue_and_out;
	}

	TCP_ECN_check_ce(tp, skb);

	if (tcp_try_rmem_schedule(sk, skb->truesize))
		goto drop;

	/* Disable header prediction. */
	tp->pred_flags = 0;
	inet_csk_schedule_ack(sk);

	SOCK_DEBUG(sk, "out of order segment: rcv_next %X seq %X - %X\n",
		   tp->rcv_nxt, TCP_SKB_CB(skb)->seq, TCP_SKB_CB(skb)->end_seq);

	skb_set_owner_r(skb, sk);

	if (!skb_peek(&tp->out_of_order_queue)) {
		/* Initial out of order segment, build 1 SACK. */
		if (tcp_is_sack(tp)) {
			tp->rx_opt.num_sacks = 1;
			tp->selective_acks[0].start_seq = TCP_SKB_CB(skb)->seq;
			tp->selective_acks[0].end_seq =
						TCP_SKB_CB(skb)->end_seq;
		}
		__skb_queue_head(&tp->out_of_order_queue, skb);
	} else {
		struct sk_buff *skb1 = skb_peek_tail(&tp->out_of_order_queue);
		u32 seq = TCP_SKB_CB(skb)->seq;
		u32 end_seq = TCP_SKB_CB(skb)->end_seq;

		if (seq == TCP_SKB_CB(skb1)->end_seq) {
			__skb_queue_after(&tp->out_of_order_queue, skb1, skb);

			if (!tp->rx_opt.num_sacks ||
			    tp->selective_acks[0].end_seq != seq)
				goto add_sack;

			/* Common case: data arrive in order after hole. */
			tp->selective_acks[0].end_seq = end_seq;
			return;
		}

		/* Find place to insert this segment. */
		while (1) {
			if (!after(TCP_SKB_CB(skb1)->seq, seq))
				break;
			if (skb_queue_is_first(&tp->out_of_order_queue, skb1)) {
				skb1 = NULL;
				break;
			}
			skb1 = skb_queue_prev(&tp->out_of_order_queue, skb1);
		}

		/* Do skb overlap to previous one? */
		if (skb1 && before(seq, TCP_SKB_CB(skb1)->end_seq)) {
			if (!after(end_seq, TCP_SKB_CB(skb1)->end_seq)) {
				/* All the bits are present. Drop. */
				__kfree_skb(skb);
				tcp_dsack_set(sk, seq, end_seq);
				goto add_sack;
			}
			if (after(seq, TCP_SKB_CB(skb1)->seq)) {
				/* Partial overlap. */
				tcp_dsack_set(sk, seq,
					      TCP_SKB_CB(skb1)->end_seq);
			} else {
				if (skb_queue_is_first(&tp->out_of_order_queue,
						       skb1))
					skb1 = NULL;
				else
					skb1 = skb_queue_prev(
						&tp->out_of_order_queue,
						skb1);
			}
		}
		if (!skb1)
			__skb_queue_head(&tp->out_of_order_queue, skb);
		else
			__skb_queue_after(&tp->out_of_order_queue, skb1, skb);

		/* And clean segments covered by new one as whole. */
		while (!skb_queue_is_last(&tp->out_of_order_queue, skb)) {
			skb1 = skb_queue_next(&tp->out_of_order_queue, skb);

			if (!after(end_seq, TCP_SKB_CB(skb1)->seq))
				break;
			if (before(end_seq, TCP_SKB_CB(skb1)->end_seq)) {
				tcp_dsack_extend(sk, TCP_SKB_CB(skb1)->seq,
						 end_seq);
				break;
			}
			__skb_unlink(skb1, &tp->out_of_order_queue);
			tcp_dsack_extend(sk, TCP_SKB_CB(skb1)->seq,
					 TCP_SKB_CB(skb1)->end_seq);
			__kfree_skb(skb1);
		}

add_sack:
		if (tcp_is_sack(tp))
			tcp_sack_new_ofo_skb(sk, seq, end_seq);
	}
}
static int packet_set_ring(struct sock *sk, union tpacket_req_u *req_u,
		int closing, int tx_ring)
{
	struct pgv *pg_vec = NULL;
	struct packet_sock *po = pkt_sk(sk);
	unsigned long *rx_owner_map = NULL;
	int was_running, order = 0;
	struct packet_ring_buffer *rb;
	struct sk_buff_head *rb_queue;
	__be16 num;
	int err;
	/* Added to avoid minimal code churn */
	struct tpacket_req *req = &req_u->req;

	rb = tx_ring ? &po->tx_ring : &po->rx_ring;
	rb_queue = tx_ring ? &sk->sk_write_queue : &sk->sk_receive_queue;

	err = -EBUSY;
	if (!closing) {
		if (atomic_read(&po->mapped))
			goto out;
		if (packet_read_pending(rb))
			goto out;
	}

	if (req->tp_block_nr) {
		unsigned int min_frame_size;

		/* Sanity tests and some calculations */
		err = -EBUSY;
		if (unlikely(rb->pg_vec))
			goto out;

		switch (po->tp_version) {
		case TPACKET_V1:
			po->tp_hdrlen = TPACKET_HDRLEN;
			break;
		case TPACKET_V2:
			po->tp_hdrlen = TPACKET2_HDRLEN;
			break;
		case TPACKET_V3:
			po->tp_hdrlen = TPACKET3_HDRLEN;
			break;
		}

		err = -EINVAL;
		if (unlikely((int)req->tp_block_size <= 0))
			goto out;
		if (unlikely(!PAGE_ALIGNED(req->tp_block_size)))
			goto out;
		min_frame_size = po->tp_hdrlen + po->tp_reserve;
		if (po->tp_version >= TPACKET_V3 &&
		    req->tp_block_size <
		    BLK_PLUS_PRIV((u64)req_u->req3.tp_sizeof_priv) + min_frame_size)
			goto out;
		if (unlikely(req->tp_frame_size < min_frame_size))
			goto out;
		if (unlikely(req->tp_frame_size & (TPACKET_ALIGNMENT - 1)))
			goto out;

		rb->frames_per_block = req->tp_block_size / req->tp_frame_size;
		if (unlikely(rb->frames_per_block == 0))
			goto out;
		if (unlikely(rb->frames_per_block > UINT_MAX / req->tp_block_nr))
			goto out;
		if (unlikely((rb->frames_per_block * req->tp_block_nr) !=
					req->tp_frame_nr))
			goto out;

		err = -ENOMEM;
		order = get_order(req->tp_block_size);
		pg_vec = alloc_pg_vec(req, order);
		if (unlikely(!pg_vec))
			goto out;
		switch (po->tp_version) {
		case TPACKET_V3:
			/* Block transmit is not supported yet */
			if (!tx_ring) {
				init_prb_bdqc(po, rb, pg_vec, req_u);
			} else {
				struct tpacket_req3 *req3 = &req_u->req3;

				if (req3->tp_retire_blk_tov ||
				    req3->tp_sizeof_priv ||
				    req3->tp_feature_req_word) {
					err = -EINVAL;
					goto out_free_pg_vec;
				}
			}
			break;
		default:
			if (!tx_ring) {
				rx_owner_map = bitmap_alloc(req->tp_frame_nr,
					GFP_KERNEL | __GFP_NOWARN | __GFP_ZERO);
				if (!rx_owner_map)
					goto out_free_pg_vec;
			}
			break;
		}
	}
	/* Done */
	else {
		err = -EINVAL;
		if (unlikely(req->tp_frame_nr))
			goto out;
	}


	/* Detach socket from network */
	spin_lock(&po->bind_lock);
	was_running = po->running;
	num = po->num;
	if (was_running) {
		po->num = 0;
		__unregister_prot_hook(sk, false);
	}
	spin_unlock(&po->bind_lock);

	synchronize_net();

	err = -EBUSY;
	mutex_lock(&po->pg_vec_lock);
	if (closing || atomic_read(&po->mapped) == 0) {
		err = 0;
		spin_lock_bh(&rb_queue->lock);
		swap(rb->pg_vec, pg_vec);
		if (po->tp_version <= TPACKET_V2)
			swap(rb->rx_owner_map, rx_owner_map);
		rb->frame_max = (req->tp_frame_nr - 1);
		rb->head = 0;
		rb->frame_size = req->tp_frame_size;
		spin_unlock_bh(&rb_queue->lock);

		swap(rb->pg_vec_order, order);
		swap(rb->pg_vec_len, req->tp_block_nr);

		rb->pg_vec_pages = req->tp_block_size/PAGE_SIZE;
		po->prot_hook.func = (po->rx_ring.pg_vec) ?
						tpacket_rcv : packet_rcv;
		skb_queue_purge(rb_queue);
		if (atomic_read(&po->mapped))
			pr_err("packet_mmap: vma is busy: %d\n",
			       atomic_read(&po->mapped));
	}
	mutex_unlock(&po->pg_vec_lock);

	spin_lock(&po->bind_lock);
	if (was_running) {
		po->num = num;
		register_prot_hook(sk);
	}
	spin_unlock(&po->bind_lock);
	if (pg_vec && (po->tp_version > TPACKET_V2)) {
		/* Because we don't support block-based V3 on tx-ring */
		if (!tx_ring)
			prb_shutdown_retire_blk_timer(po, rb_queue);
	}

out_free_pg_vec:
	bitmap_free(rx_owner_map);
	if (pg_vec)
		free_pg_vec(pg_vec, order, req->tp_block_nr);
out:
	return err;
}
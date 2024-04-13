static int sctp_sendmsg_to_asoc(struct sctp_association *asoc,
				struct msghdr *msg, size_t msg_len,
				struct sctp_transport *transport,
				struct sctp_sndrcvinfo *sinfo)
{
	struct sock *sk = asoc->base.sk;
	struct sctp_sock *sp = sctp_sk(sk);
	struct net *net = sock_net(sk);
	struct sctp_datamsg *datamsg;
	bool wait_connect = false;
	struct sctp_chunk *chunk;
	long timeo;
	int err;

	if (sinfo->sinfo_stream >= asoc->stream.outcnt) {
		err = -EINVAL;
		goto err;
	}

	if (unlikely(!SCTP_SO(&asoc->stream, sinfo->sinfo_stream)->ext)) {
		err = sctp_stream_init_ext(&asoc->stream, sinfo->sinfo_stream);
		if (err)
			goto err;
	}

	if (sp->disable_fragments && msg_len > asoc->frag_point) {
		err = -EMSGSIZE;
		goto err;
	}

	if (asoc->pmtu_pending) {
		if (sp->param_flags & SPP_PMTUD_ENABLE)
			sctp_assoc_sync_pmtu(asoc);
		asoc->pmtu_pending = 0;
	}

	if (sctp_wspace(asoc) < (int)msg_len)
		sctp_prsctp_prune(asoc, sinfo, msg_len - sctp_wspace(asoc));

	if (sk_under_memory_pressure(sk))
		sk_mem_reclaim(sk);

	if (sctp_wspace(asoc) <= 0 || !sk_wmem_schedule(sk, msg_len)) {
		timeo = sock_sndtimeo(sk, msg->msg_flags & MSG_DONTWAIT);
		err = sctp_wait_for_sndbuf(asoc, &timeo, msg_len);
		if (err)
			goto err;
	}

	if (sctp_state(asoc, CLOSED)) {
		err = sctp_primitive_ASSOCIATE(net, asoc, NULL);
		if (err)
			goto err;

		if (asoc->ep->intl_enable) {
			timeo = sock_sndtimeo(sk, 0);
			err = sctp_wait_for_connect(asoc, &timeo);
			if (err) {
				err = -ESRCH;
				goto err;
			}
		} else {
			wait_connect = true;
		}

		pr_debug("%s: we associated primitively\n", __func__);
	}

	datamsg = sctp_datamsg_from_user(asoc, sinfo, &msg->msg_iter);
	if (IS_ERR(datamsg)) {
		err = PTR_ERR(datamsg);
		goto err;
	}

	asoc->force_delay = !!(msg->msg_flags & MSG_MORE);

	list_for_each_entry(chunk, &datamsg->chunks, frag_list) {
		sctp_chunk_hold(chunk);
		sctp_set_owner_w(chunk);
		chunk->transport = transport;
	}

	err = sctp_primitive_SEND(net, asoc, datamsg);
	if (err) {
		sctp_datamsg_free(datamsg);
		goto err;
	}

	pr_debug("%s: we sent primitively\n", __func__);

	sctp_datamsg_put(datamsg);

	if (unlikely(wait_connect)) {
		timeo = sock_sndtimeo(sk, msg->msg_flags & MSG_DONTWAIT);
		sctp_wait_for_connect(asoc, &timeo);
	}

	err = msg_len;

err:
	return err;
}
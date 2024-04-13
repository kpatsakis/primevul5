CtPtr ProtocolV1::open(ceph_msg_connect_reply &reply,
                       bufferlist &authorizer_reply) {
  ldout(cct, 20) << __func__ << dendl;

  connect_seq = connect_msg.connect_seq + 1;
  peer_global_seq = connect_msg.global_seq;
  ldout(cct, 10) << __func__ << " accept success, connect_seq = " << connect_seq
                 << " in_seq=" << in_seq << ", sending READY" << dendl;

  // if it is a hard reset from peer, we don't need a round-trip to negotiate
  // in/out sequence
  if ((connect_msg.features & CEPH_FEATURE_RECONNECT_SEQ) &&
      !is_reset_from_peer) {
    reply.tag = CEPH_MSGR_TAG_SEQ;
    wait_for_seq = true;
  } else {
    reply.tag = CEPH_MSGR_TAG_READY;
    wait_for_seq = false;
    out_seq = discard_requeued_up_to(out_seq, 0);
    is_reset_from_peer = false;
    in_seq = 0;
  }

  // send READY reply
  reply.features = connection->policy.features_supported;
  reply.global_seq = messenger->get_global_seq();
  reply.connect_seq = connect_seq;
  reply.flags = 0;
  reply.authorizer_len = authorizer_reply.length();
  if (connection->policy.lossy) {
    reply.flags = reply.flags | CEPH_MSG_CONNECT_LOSSY;
  }

  connection->set_features((uint64_t)reply.features &
                           (uint64_t)connect_msg.features);
  ldout(cct, 10) << __func__ << " accept features "
                 << connection->get_features()
		 << " authorizer_protocol "
		 << connect_msg.authorizer_protocol << dendl;

  session_security.reset(
    get_auth_session_handler(cct, auth_meta->auth_method,
			     auth_meta->session_key,
			     connection->get_features()));

  bufferlist reply_bl;
  reply_bl.append((char *)&reply, sizeof(reply));

  if (reply.authorizer_len) {
    reply_bl.append(authorizer_reply.c_str(), authorizer_reply.length());
  }

  if (reply.tag == CEPH_MSGR_TAG_SEQ) {
    uint64_t s = in_seq;
    reply_bl.append((char *)&s, sizeof(s));
  }

  connection->lock.unlock();
  // Because "replacing" will prevent other connections preempt this addr,
  // it's safe that here we don't acquire Connection's lock
  ssize_t r = messenger->accept_conn(connection);

  connection->inject_delay();

  connection->lock.lock();
  replacing = false;
  if (r < 0) {
    ldout(cct, 1) << __func__ << " existing race replacing process for addr = "
                  << connection->peer_addrs->legacy_addr()
                  << " just fail later one(this)" << dendl;
    ldout(cct, 10) << "accept fault after register" << dendl;
    connection->inject_delay();
    return _fault();
  }
  if (state != ACCEPTING_WAIT_CONNECT_MSG_AUTH) {
    ldout(cct, 1) << __func__
                  << " state changed while accept_conn, it must be mark_down"
                  << dendl;
    ceph_assert(state == CLOSED || state == NONE);
    ldout(cct, 10) << "accept fault after register" << dendl;
    messenger->unregister_conn(connection);
    connection->inject_delay();
    return _fault();
  }

  return WRITE(reply_bl, handle_ready_connect_message_reply_write);
}
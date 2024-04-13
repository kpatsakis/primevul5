CtPtr ProtocolV1::handle_connect_reply_2() {
  ldout(cct, 20) << __func__ << dendl;

  if (connect_reply.tag == CEPH_MSGR_TAG_FEATURES) {
    ldout(cct, 0) << __func__ << " connect protocol feature mismatch, my "
                  << std::hex << connection->policy.features_supported
                  << " < peer " << connect_reply.features << " missing "
                  << (connect_reply.features &
                      ~connection->policy.features_supported)
                  << std::dec << dendl;
    return _fault();
  }

  if (connect_reply.tag == CEPH_MSGR_TAG_BADPROTOVER) {
    ldout(cct, 0) << __func__ << " connect protocol version mismatch, my "
                  << messenger->get_proto_version(connection->peer_type, true)
                  << " != " << connect_reply.protocol_version << dendl;
    return _fault();
  }

  if (connect_reply.tag == CEPH_MSGR_TAG_BADAUTHORIZER) {
    ldout(cct, 0) << __func__ << " connect got BADAUTHORIZER" << dendl;
    authorizer_more.clear();
    return _fault();
  }

  if (connect_reply.tag == CEPH_MSGR_TAG_RESETSESSION) {
    ldout(cct, 0) << __func__ << " connect got RESETSESSION" << dendl;
    session_reset();
    connect_seq = 0;

    // see session_reset
    connection->outgoing_bl.clear();

    return CONTINUE(send_connect_message);
  }

  if (connect_reply.tag == CEPH_MSGR_TAG_RETRY_GLOBAL) {
    global_seq = messenger->get_global_seq(connect_reply.global_seq);
    ldout(cct, 5) << __func__ << " connect got RETRY_GLOBAL "
                  << connect_reply.global_seq << " chose new " << global_seq
                  << dendl;
    return CONTINUE(send_connect_message);
  }

  if (connect_reply.tag == CEPH_MSGR_TAG_RETRY_SESSION) {
    ceph_assert(connect_reply.connect_seq > connect_seq);
    ldout(cct, 5) << __func__ << " connect got RETRY_SESSION " << connect_seq
                  << " -> " << connect_reply.connect_seq << dendl;
    connect_seq = connect_reply.connect_seq;
    return CONTINUE(send_connect_message);
  }

  if (connect_reply.tag == CEPH_MSGR_TAG_WAIT) {
    ldout(cct, 1) << __func__ << " connect got WAIT (connection race)" << dendl;
    state = WAIT;
    return _fault();
  }

  uint64_t feat_missing;
  feat_missing =
      connection->policy.features_required & ~(uint64_t)connect_reply.features;
  if (feat_missing) {
    ldout(cct, 1) << __func__ << " missing required features " << std::hex
                  << feat_missing << std::dec << dendl;
    return _fault();
  }

  if (connect_reply.tag == CEPH_MSGR_TAG_SEQ) {
    ldout(cct, 10)
        << __func__
        << " got CEPH_MSGR_TAG_SEQ, reading acked_seq and writing in_seq"
        << dendl;

    return wait_ack_seq();
  }

  if (connect_reply.tag == CEPH_MSGR_TAG_READY) {
    ldout(cct, 10) << __func__ << " got CEPH_MSGR_TAG_READY " << dendl;
  }

  return client_ready();
}
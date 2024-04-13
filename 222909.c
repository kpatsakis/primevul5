CtPtr ProtocolV1::handle_connect_reply_auth(char *buffer, int r) {
  ldout(cct, 20) << __func__ << " r=" << r << dendl;

  if (r < 0) {
    ldout(cct, 1) << __func__ << " read connect reply authorizer failed"
                  << dendl;
    return _fault();
  }

  bufferlist authorizer_reply;
  authorizer_reply.append(buffer, connect_reply.authorizer_len);

  if (connection->peer_type != CEPH_ENTITY_TYPE_MON ||
      messenger->get_myname().type() == CEPH_ENTITY_TYPE_MON) {
    auto am = auth_meta;
    bool more = (connect_reply.tag == CEPH_MSGR_TAG_CHALLENGE_AUTHORIZER);
    bufferlist auth_retry_bl;
    int r;
    connection->lock.unlock();
    if (more) {
      r = messenger->auth_client->handle_auth_reply_more(
	connection, am.get(), authorizer_reply, &auth_retry_bl);
    } else {
      // these aren't used for v1
      CryptoKey skey;
      string con_secret;
      r = messenger->auth_client->handle_auth_done(
	connection, am.get(),
	0 /* global id */, 0 /* con mode */,
	authorizer_reply,
	&skey, &con_secret);
    }
    connection->lock.lock();
    if (state != CONNECTING_SEND_CONNECT_MSG) {
      ldout(cct, 1) << __func__ << " state changed" << dendl;
      return _fault();
    }
    if (r < 0) {
      return _fault();
    }
    if (more && r == 0) {
      authorizer_more = auth_retry_bl;
      return CONTINUE(send_connect_message);
    }
  }

  return handle_connect_reply_2();
}
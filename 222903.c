CtPtr ProtocolV1::send_connect_message()
{
  state = CONNECTING_SEND_CONNECT_MSG;

  ldout(cct, 20) << __func__ << dendl;
  ceph_assert(messenger->auth_client);

  bufferlist auth_bl;
  vector<uint32_t> preferred_modes;

  if (connection->peer_type != CEPH_ENTITY_TYPE_MON ||
      messenger->get_myname().type() == CEPH_ENTITY_TYPE_MON) {
    if (authorizer_more.length()) {
      ldout(cct,10) << __func__ << " using augmented (challenge) auth payload"
		    << dendl;
      auth_bl = authorizer_more;
    } else {
      auto am = auth_meta;
      authorizer_more.clear();
      connection->lock.unlock();
      int r = messenger->auth_client->get_auth_request(
	connection, am.get(),
	&am->auth_method, &preferred_modes, &auth_bl);
      connection->lock.lock();
      if (r < 0) {
	return _fault();
      }
      if (state != CONNECTING_SEND_CONNECT_MSG) {
	ldout(cct, 1) << __func__ << " state changed!" << dendl;
	return _fault();
      }
    }
  }

  ceph_msg_connect connect;
  connect.features = connection->policy.features_supported;
  connect.host_type = messenger->get_myname().type();
  connect.global_seq = global_seq;
  connect.connect_seq = connect_seq;
  connect.protocol_version =
      messenger->get_proto_version(connection->peer_type, true);
  if (auth_bl.length()) {
    ldout(cct, 10) << __func__
                   << " connect_msg.authorizer_len=" << auth_bl.length()
                   << " protocol=" << auth_meta->auth_method << dendl;
    connect.authorizer_protocol = auth_meta->auth_method;
    connect.authorizer_len = auth_bl.length();
  } else {
    connect.authorizer_protocol = 0;
    connect.authorizer_len = 0;
  }

  connect.flags = 0;
  if (connection->policy.lossy) {
    connect.flags |=
        CEPH_MSG_CONNECT_LOSSY;  // this is fyi, actually, server decides!
  }

  bufferlist bl;
  bl.append((char *)&connect, sizeof(connect));
  if (auth_bl.length()) {
    bl.append(auth_bl.c_str(), auth_bl.length());
  }

  ldout(cct, 10) << __func__ << " connect sending gseq=" << global_seq
                 << " cseq=" << connect_seq
                 << " proto=" << connect.protocol_version << dendl;

  return WRITE(bl, handle_connect_message_write);
}
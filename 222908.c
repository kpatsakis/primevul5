ssize_t ProtocolV1::write_message(Message *m, bufferlist &bl, bool more) {
  FUNCTRACE(cct);
  ceph_assert(connection->center->in_thread());
  m->set_seq(++out_seq);

  if (messenger->crcflags & MSG_CRC_HEADER) {
    m->calc_header_crc();
  }

  ceph_msg_header &header = m->get_header();
  ceph_msg_footer &footer = m->get_footer();

  // TODO: let sign_message could be reentry?
  // Now that we have all the crcs calculated, handle the
  // digital signature for the message, if the AsyncConnection has session
  // security set up.  Some session security options do not
  // actually calculate and check the signature, but they should
  // handle the calls to sign_message and check_signature.  PLR
  if (session_security.get() == NULL) {
    ldout(cct, 20) << __func__ << " no session security" << dendl;
  } else {
    if (session_security->sign_message(m)) {
      ldout(cct, 20) << __func__ << " failed to sign m=" << m
                     << "): sig = " << footer.sig << dendl;
    } else {
      ldout(cct, 20) << __func__ << " signed m=" << m
                     << "): sig = " << footer.sig << dendl;
    }
  }

  connection->outgoing_bl.append(CEPH_MSGR_TAG_MSG);
  connection->outgoing_bl.append((char *)&header, sizeof(header));

  ldout(cct, 20) << __func__ << " sending message type=" << header.type
                 << " src " << entity_name_t(header.src)
                 << " front=" << header.front_len << " data=" << header.data_len
                 << " off " << header.data_off << dendl;

  if ((bl.length() <= ASYNC_COALESCE_THRESHOLD) && (bl.get_num_buffers() > 1)) {
    for (const auto &pb : bl.buffers()) {
      connection->outgoing_bl.append((char *)pb.c_str(), pb.length());
    }
  } else {
    connection->outgoing_bl.claim_append(bl);
  }

  // send footer; if receiver doesn't support signatures, use the old footer
  // format
  ceph_msg_footer_old old_footer;
  if (connection->has_feature(CEPH_FEATURE_MSG_AUTH)) {
    connection->outgoing_bl.append((char *)&footer, sizeof(footer));
  } else {
    if (messenger->crcflags & MSG_CRC_HEADER) {
      old_footer.front_crc = footer.front_crc;
      old_footer.middle_crc = footer.middle_crc;
    } else {
      old_footer.front_crc = old_footer.middle_crc = 0;
    }
    old_footer.data_crc =
        messenger->crcflags & MSG_CRC_DATA ? footer.data_crc : 0;
    old_footer.flags = footer.flags;
    connection->outgoing_bl.append((char *)&old_footer, sizeof(old_footer));
  }

  m->trace.event("async writing message");
  ldout(cct, 20) << __func__ << " sending " << m->get_seq() << " " << m
                 << dendl;
  ssize_t total_send_size = connection->outgoing_bl.length();
  ssize_t rc = connection->_try_send(more);
  if (rc < 0) {
    ldout(cct, 1) << __func__ << " error sending " << m << ", "
                  << cpp_strerror(rc) << dendl;
  } else {
    connection->logger->inc(
        l_msgr_send_bytes, total_send_size - connection->outgoing_bl.length());
    ldout(cct, 10) << __func__ << " sending " << m
                   << (rc ? " continuely." : " done.") << dendl;
  }

#if defined(WITH_EVENTTRACE)
  if (m->get_type() == CEPH_MSG_OSD_OP)
    OID_EVENT_TRACE_WITH_MSG(m, "SEND_MSG_OSD_OP_END", false);
  else if (m->get_type() == CEPH_MSG_OSD_OPREPLY)
    OID_EVENT_TRACE_WITH_MSG(m, "SEND_MSG_OSD_OPREPLY_END", false);
#endif
  m->put();

  return rc;
}
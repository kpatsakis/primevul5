CtPtr ProtocolV1::handle_message(char *buffer, int r) {
  ldout(cct, 20) << __func__ << " r=" << r << dendl;

  if (r < 0) {
    ldout(cct, 1) << __func__ << " read tag failed" << dendl;
    return _fault();
  }

  char tag = buffer[0];
  ldout(cct, 20) << __func__ << " process tag " << (int)tag << dendl;

  if (tag == CEPH_MSGR_TAG_KEEPALIVE) {
    ldout(cct, 20) << __func__ << " got KEEPALIVE" << dendl;
    connection->set_last_keepalive(ceph_clock_now());
  } else if (tag == CEPH_MSGR_TAG_KEEPALIVE2) {
    return READ(sizeof(ceph_timespec), handle_keepalive2);
  } else if (tag == CEPH_MSGR_TAG_KEEPALIVE2_ACK) {
    return READ(sizeof(ceph_timespec), handle_keepalive2_ack);
  } else if (tag == CEPH_MSGR_TAG_ACK) {
    return READ(sizeof(ceph_le64), handle_tag_ack);
  } else if (tag == CEPH_MSGR_TAG_MSG) {
    recv_stamp = ceph_clock_now();
    ldout(cct, 20) << __func__ << " begin MSG" << dendl;
    return READ(sizeof(ceph_msg_header), handle_message_header);
  } else if (tag == CEPH_MSGR_TAG_CLOSE) {
    ldout(cct, 20) << __func__ << " got CLOSE" << dendl;
    stop();
  } else {
    ldout(cct, 0) << __func__ << " bad tag " << (int)tag << dendl;
    return _fault();
  }
  return nullptr;
}
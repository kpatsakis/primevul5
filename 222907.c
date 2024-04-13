CtPtr ProtocolV1::handle_tag_ack(char *buffer, int r) {
  ldout(cct, 20) << __func__ << " r=" << r << dendl;

  if (r < 0) {
    ldout(cct, 1) << __func__ << " read ack seq failed" << dendl;
    return _fault();
  }

  ceph_le64 seq;
  seq = *(ceph_le64 *)buffer;
  ldout(cct, 20) << __func__ << " got ACK" << dendl;

  ldout(cct, 15) << __func__ << " got ack seq " << seq << dendl;
  // trim sent list
  static const int max_pending = 128;
  int i = 0;
  auto now = ceph::mono_clock::now();
  Message *pending[max_pending];
  connection->write_lock.lock();
  while (!sent.empty() && sent.front()->get_seq() <= seq && i < max_pending) {
    Message *m = sent.front();
    sent.pop_front();
    pending[i++] = m;
    ldout(cct, 10) << __func__ << " got ack seq " << seq
                   << " >= " << m->get_seq() << " on " << m << " " << *m
                   << dendl;
  }
  connection->write_lock.unlock();
  connection->logger->tinc(l_msgr_handle_ack_lat, ceph::mono_clock::now() - now);
  for (int k = 0; k < i; k++) {
    pending[k]->put();
  }

  return CONTINUE(wait_message);
}
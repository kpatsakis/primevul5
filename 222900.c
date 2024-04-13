void ProtocolV1::requeue_sent() {
  write_in_progress = false;
  if (sent.empty()) {
    return;
  }

  list<pair<bufferlist, Message *> > &rq = out_q[CEPH_MSG_PRIO_HIGHEST];
  out_seq -= sent.size();
  while (!sent.empty()) {
    Message *m = sent.back();
    sent.pop_back();
    ldout(cct, 10) << __func__ << " " << *m << " for resend "
                   << " (" << m->get_seq() << ")" << dendl;
    m->clear_payload();
    rq.push_front(make_pair(bufferlist(), m));
  }
}
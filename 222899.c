Message *ProtocolV1::_get_next_outgoing(bufferlist *bl) {
  Message *m = 0;
  if (!out_q.empty()) {
    map<int, list<pair<bufferlist, Message *> > >::reverse_iterator it =
        out_q.rbegin();
    ceph_assert(!it->second.empty());
    list<pair<bufferlist, Message *> >::iterator p = it->second.begin();
    m = p->second;
    if (p->first.length() && bl) {
      assert(bl->length() == 0);
      bl->swap(p->first);
    }
    it->second.erase(p);
    if (it->second.empty()) out_q.erase(it->first);
  }
  return m;
}
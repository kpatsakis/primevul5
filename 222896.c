ProtocolV1::~ProtocolV1() {
  ceph_assert(out_q.empty());
  ceph_assert(sent.empty());

  delete[] temp_buffer;
}
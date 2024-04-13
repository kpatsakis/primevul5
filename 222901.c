void ProtocolV1::prepare_send_message(uint64_t features, Message *m,
                                      bufferlist &bl) {
  ldout(cct, 20) << __func__ << " m " << *m << dendl;

  // associate message with Connection (for benefit of encode_payload)
  ldout(cct, 20) << __func__ << (m->empty_payload() ? " encoding features " : " half-reencoding features ")
		 << features << " " << m  << " " << *m << dendl;

  // encode and copy out of *m
  // in write_message we update header.seq and need recalc crc
  // so skip calc header in encode function.
  m->encode(features, messenger->crcflags, true);

  bl.append(m->get_payload());
  bl.append(m->get_middle());
  bl.append(m->get_data());
}
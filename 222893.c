ProtocolV1::ProtocolV1(AsyncConnection *connection)
    : Protocol(1, connection),
      temp_buffer(nullptr),
      can_write(WriteStatus::NOWRITE),
      keepalive(false),
      connect_seq(0),
      peer_global_seq(0),
      msg_left(0),
      cur_msg_size(0),
      replacing(false),
      is_reset_from_peer(false),
      once_ready(false),
      state(NONE),
      global_seq(0),
      wait_for_seq(false) {
  temp_buffer = new char[4096];
}
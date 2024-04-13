void ProtocolV1::connect() {
  this->state = START_CONNECT;

  // reset connect state variables
  authorizer_buf.clear();
  // FIPS zeroization audit 20191115: these memsets are not security related.
  memset(&connect_msg, 0, sizeof(connect_msg));
  memset(&connect_reply, 0, sizeof(connect_reply));

  global_seq = messenger->get_global_seq();
}
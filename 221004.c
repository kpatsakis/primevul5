proto_reg_handoff_dnp3(void)
{
  /* register as heuristic dissector for both TCP and UDP */
  heur_dissector_add("tcp", dissect_dnp3_tcp_heur, "DNP 3.0 over TCP", "dnp3_tcp", proto_dnp3, HEURISTIC_DISABLE);
  heur_dissector_add("udp", dissect_dnp3_udp_heur, "DNP 3.0 over UDP", "dnp3_udp", proto_dnp3, HEURISTIC_DISABLE);

  dissector_add_uint_with_preference("tcp.port", TCP_PORT_DNP, dnp3_tcp_handle);
  dissector_add_uint_with_preference("udp.port", UDP_PORT_DNP, dnp3_udp_handle);
  dissector_add_for_decode_as("rtacser.data", dnp3_udp_handle);

  ssl_dissector_add(TCP_PORT_DNP_TLS, dnp3_tcp_handle);
}
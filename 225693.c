proto_reg_handoff_tcp(void)
{
    capture_dissector_handle_t tcp_cap_handle;

    dissector_add_uint("ip.proto", IP_PROTO_TCP, tcp_handle);
    dissector_add_for_decode_as_with_preference("udp.port", tcp_handle);
    data_handle = find_dissector("data");
    sport_handle = find_dissector("sport");
    tcp_tap = register_tap("tcp");
    tcp_follow_tap = register_tap("tcp_follow");

    tcp_cap_handle = create_capture_dissector_handle(capture_tcp, proto_tcp);
    capture_dissector_add_uint("ip.proto", IP_PROTO_TCP, tcp_cap_handle);

    /* Create dissection function handles for all TCP options */
    dissector_add_uint("tcp.option", TCPOPT_TIMESTAMP, create_dissector_handle( dissect_tcpopt_timestamp, proto_tcp_option_timestamp ));
    dissector_add_uint("tcp.option", TCPOPT_MSS, create_dissector_handle( dissect_tcpopt_mss, proto_tcp_option_mss ));
    dissector_add_uint("tcp.option", TCPOPT_WINDOW, create_dissector_handle( dissect_tcpopt_wscale, proto_tcp_option_wscale ));
    dissector_add_uint("tcp.option", TCPOPT_SACK_PERM, create_dissector_handle( dissect_tcpopt_sack_perm, proto_tcp_option_sack_perm ));
    dissector_add_uint("tcp.option", TCPOPT_SACK, create_dissector_handle( dissect_tcpopt_sack, proto_tcp_option_sack ));
    dissector_add_uint("tcp.option", TCPOPT_ECHO, create_dissector_handle( dissect_tcpopt_echo, proto_tcp_option_echo ));
    dissector_add_uint("tcp.option", TCPOPT_ECHOREPLY, create_dissector_handle( dissect_tcpopt_echo, proto_tcp_option_echoreply ));
    dissector_add_uint("tcp.option", TCPOPT_CC, create_dissector_handle( dissect_tcpopt_cc, proto_tcp_option_cc ));
    dissector_add_uint("tcp.option", TCPOPT_CCNEW, create_dissector_handle( dissect_tcpopt_cc, proto_tcp_option_cc_new ));
    dissector_add_uint("tcp.option", TCPOPT_CCECHO, create_dissector_handle( dissect_tcpopt_cc, proto_tcp_option_cc_echo ));
    dissector_add_uint("tcp.option", TCPOPT_MD5, create_dissector_handle( dissect_tcpopt_md5, proto_tcp_option_md5 ));
    dissector_add_uint("tcp.option", TCPOPT_SCPS, create_dissector_handle( dissect_tcpopt_scps, proto_tcp_option_scps ));
    dissector_add_uint("tcp.option", TCPOPT_SNACK, create_dissector_handle( dissect_tcpopt_snack, proto_tcp_option_snack ));
    dissector_add_uint("tcp.option", TCPOPT_RECBOUND, create_dissector_handle( dissect_tcpopt_recbound, proto_tcp_option_scpsrec ));
    dissector_add_uint("tcp.option", TCPOPT_CORREXP, create_dissector_handle( dissect_tcpopt_correxp, proto_tcp_option_scpscor ));
    dissector_add_uint("tcp.option", TCPOPT_QS, create_dissector_handle( dissect_tcpopt_qs, proto_tcp_option_qs ));
    dissector_add_uint("tcp.option", TCPOPT_USER_TO, create_dissector_handle( dissect_tcpopt_user_to, proto_tcp_option_user_to ));
    dissector_add_uint("tcp.option", TCPOPT_TFO, create_dissector_handle( dissect_tcpopt_tfo, proto_tcp_option_tfo ));
    dissector_add_uint("tcp.option", TCPOPT_RVBD_PROBE, create_dissector_handle( dissect_tcpopt_rvbd_probe, proto_tcp_option_rvbd_probe ));
    dissector_add_uint("tcp.option", TCPOPT_RVBD_TRPY, create_dissector_handle( dissect_tcpopt_rvbd_trpy, proto_tcp_option_rvbd_trpy ));
    dissector_add_uint("tcp.option", TCPOPT_EXP_FD, create_dissector_handle( dissect_tcpopt_exp, proto_tcp_option_exp ));
    dissector_add_uint("tcp.option", TCPOPT_EXP_FE, create_dissector_handle( dissect_tcpopt_exp, proto_tcp_option_exp ));
    dissector_add_uint("tcp.option", TCPOPT_MPTCP, create_dissector_handle( dissect_tcpopt_mptcp, proto_mptcp ));
    /* Common handle for all the unknown/unsupported TCP options */
    tcp_opt_unknown_handle = create_dissector_handle( dissect_tcpopt_unknown, proto_tcp_option_unknown );

    mptcp_tap = register_tap("mptcp");
    exported_pdu_tap = find_tap_id(EXPORT_PDU_TAP_NAME_LAYER_4);

    proto_ip = proto_get_id_by_filter_name("ip");
    proto_icmp = proto_get_id_by_filter_name("icmp");
}
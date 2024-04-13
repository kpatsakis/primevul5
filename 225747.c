proto_register_tcp(void)
{
    static hf_register_info hf[] = {

        { &hf_tcp_srcport,
        { "Source Port",        "tcp.srcport", FT_UINT16, BASE_PT_TCP, NULL, 0x0,
            NULL, HFILL }},

        { &hf_tcp_dstport,
        { "Destination Port",       "tcp.dstport", FT_UINT16, BASE_PT_TCP, NULL, 0x0,
            NULL, HFILL }},

        { &hf_tcp_port,
        { "Source or Destination Port", "tcp.port", FT_UINT16, BASE_PT_TCP, NULL, 0x0,
            NULL, HFILL }},

        { &hf_tcp_stream,
        { "Stream index",       "tcp.stream", FT_UINT32, BASE_DEC, NULL, 0x0,
            NULL, HFILL }},

        { &hf_tcp_seq,
        { "Sequence number",        "tcp.seq", FT_UINT32, BASE_DEC, NULL, 0x0,
            NULL, HFILL }},

        { &hf_tcp_nxtseq,
        { "Next sequence number",   "tcp.nxtseq", FT_UINT32, BASE_DEC, NULL, 0x0,
            NULL, HFILL }},

        { &hf_tcp_ack,
        { "Acknowledgment number", "tcp.ack", FT_UINT32, BASE_DEC, NULL, 0x0,
            NULL, HFILL }},

        { &hf_tcp_hdr_len,
        { "Header Length",      "tcp.hdr_len", FT_UINT8, BASE_DEC, NULL, 0x0,
            NULL, HFILL }},

        { &hf_tcp_flags,
        { "Flags",          "tcp.flags", FT_UINT16, BASE_HEX, NULL, TH_MASK,
            "Flags (12 bits)", HFILL }},

        { &hf_tcp_flags_res,
        { "Reserved",            "tcp.flags.res", FT_BOOLEAN, 12, TFS(&tfs_set_notset), TH_RES,
            "Three reserved bits (must be zero)", HFILL }},

        { &hf_tcp_flags_ns,
        { "Nonce", "tcp.flags.ns", FT_BOOLEAN, 12, TFS(&tfs_set_notset), TH_NS,
            "ECN concealment protection (RFC 3540)", HFILL }},

        { &hf_tcp_flags_cwr,
        { "Congestion Window Reduced (CWR)",            "tcp.flags.cwr", FT_BOOLEAN, 12, TFS(&tfs_set_notset), TH_CWR,
            NULL, HFILL }},

        { &hf_tcp_flags_ecn,
        { "ECN-Echo",           "tcp.flags.ecn", FT_BOOLEAN, 12, TFS(&tfs_set_notset), TH_ECN,
            NULL, HFILL }},

        { &hf_tcp_flags_urg,
        { "Urgent",         "tcp.flags.urg", FT_BOOLEAN, 12, TFS(&tfs_set_notset), TH_URG,
            NULL, HFILL }},

        { &hf_tcp_flags_ack,
        { "Acknowledgment",        "tcp.flags.ack", FT_BOOLEAN, 12, TFS(&tfs_set_notset), TH_ACK,
            NULL, HFILL }},

        { &hf_tcp_flags_push,
        { "Push",           "tcp.flags.push", FT_BOOLEAN, 12, TFS(&tfs_set_notset), TH_PUSH,
            NULL, HFILL }},

        { &hf_tcp_flags_reset,
        { "Reset",          "tcp.flags.reset", FT_BOOLEAN, 12, TFS(&tfs_set_notset), TH_RST,
            NULL, HFILL }},

        { &hf_tcp_flags_syn,
        { "Syn",            "tcp.flags.syn", FT_BOOLEAN, 12, TFS(&tfs_set_notset), TH_SYN,
            NULL, HFILL }},

        { &hf_tcp_flags_fin,
        { "Fin",            "tcp.flags.fin", FT_BOOLEAN, 12, TFS(&tfs_set_notset), TH_FIN,
            NULL, HFILL }},

        { &hf_tcp_flags_str,
        { "TCP Flags",          "tcp.flags.str", FT_STRING, STR_UNICODE, NULL, 0x0,
            NULL, HFILL }},

        { &hf_tcp_window_size_value,
        { "Window size value",        "tcp.window_size_value", FT_UINT16, BASE_DEC, NULL, 0x0,
            "The window size value from the TCP header", HFILL }},

        /* 32 bits so we can present some values adjusted to window scaling */
        { &hf_tcp_window_size,
        { "Calculated window size",        "tcp.window_size", FT_UINT32, BASE_DEC, NULL, 0x0,
            "The scaled window size (if scaling has been used)", HFILL }},

        { &hf_tcp_window_size_scalefactor,
        { "Window size scaling factor", "tcp.window_size_scalefactor", FT_INT32, BASE_DEC, NULL, 0x0,
            "The window size scaling factor (-1 when unknown, -2 when no scaling is used)", HFILL }},

        { &hf_tcp_checksum,
        { "Checksum",           "tcp.checksum", FT_UINT16, BASE_HEX, NULL, 0x0,
            "Details at: http://www.wireshark.org/docs/wsug_html_chunked/ChAdvChecksums.html", HFILL }},

        { &hf_tcp_checksum_status,
        { "Checksum Status",      "tcp.checksum.status", FT_UINT8, BASE_NONE, VALS(proto_checksum_vals), 0x0,
            NULL, HFILL }},

        { &hf_tcp_checksum_calculated,
        { "Calculated Checksum", "tcp.checksum_calculated", FT_UINT16, BASE_HEX, NULL, 0x0,
            "The expected TCP checksum field as calculated from the TCP segment", HFILL }},

        { &hf_tcp_analysis,
        { "SEQ/ACK analysis",   "tcp.analysis", FT_NONE, BASE_NONE, NULL, 0x0,
            "This frame has some of the TCP analysis shown", HFILL }},

        { &hf_tcp_analysis_flags,
        { "TCP Analysis Flags",     "tcp.analysis.flags", FT_NONE, BASE_NONE, NULL, 0x0,
            "This frame has some of the TCP analysis flags set", HFILL }},

        { &hf_tcp_analysis_duplicate_ack,
        { "Duplicate ACK",      "tcp.analysis.duplicate_ack", FT_NONE, BASE_NONE, NULL, 0x0,
            "This is a duplicate ACK", HFILL }},

        { &hf_tcp_analysis_duplicate_ack_num,
        { "Duplicate ACK #",        "tcp.analysis.duplicate_ack_num", FT_UINT32, BASE_DEC, NULL, 0x0,
            "This is duplicate ACK number #", HFILL }},

        { &hf_tcp_analysis_duplicate_ack_frame,
        { "Duplicate to the ACK in frame",      "tcp.analysis.duplicate_ack_frame", FT_FRAMENUM, BASE_NONE, FRAMENUM_TYPE(FT_FRAMENUM_DUP_ACK), 0x0,
            "This is a duplicate to the ACK in frame #", HFILL }},

        { &hf_tcp_continuation_to,
        { "This is a continuation to the PDU in frame",     "tcp.continuation_to", FT_FRAMENUM, BASE_NONE, NULL, 0x0,
            "This is a continuation to the PDU in frame #", HFILL }},

        { &hf_tcp_len,
          { "TCP Segment Len",            "tcp.len", FT_UINT32, BASE_DEC, NULL, 0x0,
            NULL, HFILL}},

        { &hf_tcp_analysis_acks_frame,
          { "This is an ACK to the segment in frame",            "tcp.analysis.acks_frame", FT_FRAMENUM, BASE_NONE, FRAMENUM_TYPE(FT_FRAMENUM_ACK), 0x0,
            "Which previous segment is this an ACK for", HFILL}},

        { &hf_tcp_analysis_bytes_in_flight,
          { "Bytes in flight",            "tcp.analysis.bytes_in_flight", FT_UINT32, BASE_DEC, NULL, 0x0,
            "How many bytes are now in flight for this connection", HFILL}},

        { &hf_tcp_analysis_push_bytes_sent,
          { "Bytes sent since last PSH flag",            "tcp.analysis.push_bytes_sent", FT_UINT32, BASE_DEC, NULL, 0x0,
            "How many bytes have been sent since the last PSH flag", HFILL}},

        { &hf_tcp_analysis_ack_rtt,
          { "The RTT to ACK the segment was",            "tcp.analysis.ack_rtt", FT_RELATIVE_TIME, BASE_NONE, NULL, 0x0,
            "How long time it took to ACK the segment (RTT)", HFILL}},

        { &hf_tcp_analysis_first_rtt,
          { "iRTT",            "tcp.analysis.initial_rtt", FT_RELATIVE_TIME, BASE_NONE, NULL, 0x0,
            "How long it took for the SYN to ACK handshake (iRTT)", HFILL}},

        { &hf_tcp_analysis_rto,
          { "The RTO for this segment was",            "tcp.analysis.rto", FT_RELATIVE_TIME, BASE_NONE, NULL, 0x0,
            "How long transmission was delayed before this segment was retransmitted (RTO)", HFILL}},

        { &hf_tcp_analysis_rto_frame,
          { "RTO based on delta from frame", "tcp.analysis.rto_frame", FT_FRAMENUM, BASE_NONE, NULL, 0x0,
            "This is the frame we measure the RTO from", HFILL }},

        { &hf_tcp_urgent_pointer,
        { "Urgent pointer",     "tcp.urgent_pointer", FT_UINT16, BASE_DEC, NULL, 0x0,
            NULL, HFILL }},

        { &hf_tcp_segment_overlap,
        { "Segment overlap",    "tcp.segment.overlap", FT_BOOLEAN, BASE_NONE, NULL, 0x0,
            "Segment overlaps with other segments", HFILL }},

        { &hf_tcp_segment_overlap_conflict,
        { "Conflicting data in segment overlap",    "tcp.segment.overlap.conflict", FT_BOOLEAN, BASE_NONE, NULL, 0x0,
            "Overlapping segments contained conflicting data", HFILL }},

        { &hf_tcp_segment_multiple_tails,
        { "Multiple tail segments found",   "tcp.segment.multipletails", FT_BOOLEAN, BASE_NONE, NULL, 0x0,
            "Several tails were found when reassembling the pdu", HFILL }},

        { &hf_tcp_segment_too_long_fragment,
        { "Segment too long",   "tcp.segment.toolongfragment", FT_BOOLEAN, BASE_NONE, NULL, 0x0,
            "Segment contained data past end of the pdu", HFILL }},

        { &hf_tcp_segment_error,
        { "Reassembling error", "tcp.segment.error", FT_FRAMENUM, BASE_NONE, NULL, 0x0,
            "Reassembling error due to illegal segments", HFILL }},

        { &hf_tcp_segment_count,
        { "Segment count", "tcp.segment.count", FT_UINT32, BASE_DEC, NULL, 0x0,
            NULL, HFILL }},

        { &hf_tcp_segment,
        { "TCP Segment", "tcp.segment", FT_FRAMENUM, BASE_NONE, NULL, 0x0,
            NULL, HFILL }},

        { &hf_tcp_segments,
        { "Reassembled TCP Segments", "tcp.segments", FT_NONE, BASE_NONE, NULL, 0x0,
            "TCP Segments", HFILL }},

        { &hf_tcp_reassembled_in,
        { "Reassembled PDU in frame", "tcp.reassembled_in", FT_FRAMENUM, BASE_NONE, NULL, 0x0,
            "The PDU that doesn't end in this segment is reassembled in this frame", HFILL }},

        { &hf_tcp_reassembled_length,
        { "Reassembled TCP length", "tcp.reassembled.length", FT_UINT32, BASE_DEC, NULL, 0x0,
            "The total length of the reassembled payload", HFILL }},

        { &hf_tcp_reassembled_data,
        { "Reassembled TCP Data", "tcp.reassembled.data", FT_BYTES, BASE_NONE, NULL, 0x0,
            "The reassembled payload", HFILL }},

        { &hf_tcp_option_kind,
          { "Kind", "tcp.option_kind", FT_UINT8,
            BASE_DEC|BASE_EXT_STRING, &tcp_option_kind_vs_ext, 0x0, "This TCP option's kind", HFILL }},

        { &hf_tcp_option_len,
          { "Length", "tcp.option_len", FT_UINT8,
            BASE_DEC, NULL, 0x0, "Length of this TCP option in bytes (including kind and length fields)", HFILL }},

        { &hf_tcp_options,
          { "TCP Options", "tcp.options", FT_BYTES,
            BASE_NONE, NULL, 0x0, NULL, HFILL }},

        { &hf_tcp_option_mss_val,
          { "MSS Value", "tcp.options.mss_val", FT_UINT16,
            BASE_DEC, NULL, 0x0, NULL, HFILL}},

        { &hf_tcp_option_wscale_shift,
          { "Shift count", "tcp.options.wscale.shift", FT_UINT8,
            BASE_DEC, NULL, 0x0, "Logarithmically encoded power of 2 scale factor", HFILL}},

        { &hf_tcp_option_wscale_multiplier,
          { "Multiplier", "tcp.options.wscale.multiplier",  FT_UINT16,
            BASE_DEC, NULL, 0x0, "Multiply segment window size by this for scaled window size", HFILL}},

        { &hf_tcp_option_exp_data,
          { "Data", "tcp.options.experimental.data", FT_BYTES,
            BASE_NONE, NULL, 0x0, NULL, HFILL}},

        { &hf_tcp_option_exp_magic_number,
          { "Magic Number", "tcp.options.experimental.magic_number", FT_UINT16,
            BASE_HEX, NULL, 0x0, NULL, HFILL}},

        { &hf_tcp_option_unknown_payload,
          { "Payload", "tcp.options.unknown.payload", FT_BYTES,
            BASE_NONE, NULL, 0x0, NULL, HFILL}},

        { &hf_tcp_option_sack_sle,
          {"TCP SACK Left Edge", "tcp.options.sack_le", FT_UINT32,
           BASE_DEC, NULL, 0x0, NULL, HFILL}},

        { &hf_tcp_option_sack_sre,
          {"TCP SACK Right Edge", "tcp.options.sack_re", FT_UINT32,
           BASE_DEC, NULL, 0x0, NULL, HFILL}},

        { &hf_tcp_option_sack_range_count,
          { "TCP SACK Count", "tcp.options.sack.count", FT_UINT8,
            BASE_DEC, NULL, 0x0, NULL, HFILL}},

        { &hf_tcp_option_echo,
          { "TCP Echo Option", "tcp.options.echo_value", FT_UINT32,
            BASE_DEC, NULL, 0x0, "TCP Sack Echo", HFILL}},

        { &hf_tcp_option_timestamp_tsval,
          { "Timestamp value", "tcp.options.timestamp.tsval", FT_UINT32,
            BASE_DEC, NULL, 0x0, "Value of sending machine's timestamp clock", HFILL}},

        { &hf_tcp_option_timestamp_tsecr,
          { "Timestamp echo reply", "tcp.options.timestamp.tsecr", FT_UINT32,
            BASE_DEC, NULL, 0x0, "Echoed timestamp from remote machine", HFILL}},

        { &hf_tcp_option_mptcp_subtype,
          { "Multipath TCP subtype", "tcp.options.mptcp.subtype", FT_UINT8,
            BASE_DEC, VALS(mptcp_subtype_vs), 0xF0, NULL, HFILL}},

        { &hf_tcp_option_mptcp_version,
          { "Multipath TCP version", "tcp.options.mptcp.version", FT_UINT8,
            BASE_DEC, NULL, 0x0F, NULL, HFILL}},

        { &hf_tcp_option_mptcp_reserved,
          { "Reserved", "tcp.options.mptcp.reserved", FT_UINT16,
            BASE_HEX, NULL, 0x0FFF, NULL, HFILL}},

        { &hf_tcp_option_mptcp_flags,
          { "Multipath TCP flags", "tcp.options.mptcp.flags", FT_UINT8,
            BASE_HEX, NULL, 0x0, NULL, HFILL}},

        { &hf_tcp_option_mptcp_backup_flag,
          { "Backup flag", "tcp.options.mptcp.backup.flag", FT_UINT8,
            BASE_DEC, NULL, 0x01, NULL, HFILL}},

        { &hf_tcp_option_mptcp_checksum_flag,
          { "Checksum required", "tcp.options.mptcp.checksumreq.flags", FT_UINT8,
            BASE_DEC, NULL, MPTCP_CHECKSUM_MASK, NULL, HFILL}},

        { &hf_tcp_option_mptcp_B_flag,
          { "Extensibility", "tcp.options.mptcp.extensibility.flag", FT_UINT8,
            BASE_DEC, NULL, 0x40, NULL, HFILL}},

        { &hf_tcp_option_mptcp_H_flag,
          { "Use HMAC-SHA1", "tcp.options.mptcp.sha1.flag", FT_UINT8,
            BASE_DEC, NULL, 0x01, NULL, HFILL}},

        { &hf_tcp_option_mptcp_F_flag,
          { "DATA_FIN", "tcp.options.mptcp.datafin.flag", FT_UINT8,
            BASE_DEC, NULL, MPTCP_DSS_FLAG_DATA_FIN_PRESENT, NULL, HFILL}},

        { &hf_tcp_option_mptcp_m_flag,
          { "Data Sequence Number is 8 octets", "tcp.options.mptcp.dseqn8.flag", FT_UINT8,
            BASE_DEC, NULL, MPTCP_DSS_FLAG_DSN_8BYTES, NULL, HFILL}},

        { &hf_tcp_option_mptcp_M_flag,
          { "Data Sequence Number, Subflow Sequence Number, Data-level Length, Checksum present", "tcp.options.mptcp.dseqnpresent.flag", FT_UINT8,
            BASE_DEC, NULL, MPTCP_DSS_FLAG_MAPPING_PRESENT, NULL, HFILL}},

        { &hf_tcp_option_mptcp_a_flag,
          { "Data ACK is 8 octets", "tcp.options.mptcp.dataack8.flag", FT_UINT8,
            BASE_DEC, NULL, MPTCP_DSS_FLAG_DATA_ACK_8BYTES, NULL, HFILL}},

        { &hf_tcp_option_mptcp_A_flag,
          { "Data ACK is present", "tcp.options.mptcp.dataackpresent.flag", FT_UINT8,
            BASE_DEC, NULL, MPTCP_DSS_FLAG_DATA_ACK_PRESENT, NULL, HFILL}},

        { &hf_tcp_option_mptcp_reserved_flag,
          { "Reserved", "tcp.options.mptcp.reserved.flag", FT_UINT8,
            BASE_HEX, NULL, 0x3E, NULL, HFILL}},

        { &hf_tcp_option_mptcp_address_id,
          { "Address ID", "tcp.options.mptcp.addrid", FT_UINT8,
            BASE_DEC, NULL, 0x0, NULL, HFILL}},

        { &hf_tcp_option_mptcp_sender_key,
          { "Sender's Key", "tcp.options.mptcp.sendkey", FT_UINT64,
            BASE_DEC, NULL, 0x0, NULL, HFILL}},

        { &hf_tcp_option_mptcp_recv_key,
          { "Receiver's Key", "tcp.options.mptcp.recvkey", FT_UINT64,
            BASE_DEC, NULL, 0x0, NULL, HFILL}},

        { &hf_tcp_option_mptcp_recv_token,
          { "Receiver's Token", "tcp.options.mptcp.recvtok", FT_UINT32,
            BASE_DEC, NULL, 0x0, NULL, HFILL}},

        { &hf_tcp_option_mptcp_sender_rand,
          { "Sender's Random Number", "tcp.options.mptcp.sendrand", FT_UINT32,
            BASE_DEC, NULL, 0x0, NULL, HFILL}},

        { &hf_tcp_option_mptcp_sender_trunc_hmac,
          { "Sender's Truncated HMAC", "tcp.options.mptcp.sendtrunchmac", FT_UINT64,
            BASE_DEC, NULL, 0x0, NULL, HFILL}},

        { &hf_tcp_option_mptcp_sender_hmac,
          { "Sender's HMAC", "tcp.options.mptcp.sendhmac", FT_BYTES,
            BASE_NONE, NULL, 0x0, NULL, HFILL}},

        { &hf_tcp_option_mptcp_addaddr_trunc_hmac,
          { "Truncated HMAC", "tcp.options.mptcp.addaddrtrunchmac", FT_UINT64,
            BASE_DEC, NULL, 0x0, NULL, HFILL}},

        { &hf_tcp_option_mptcp_data_ack_raw,
          { "Original MPTCP Data ACK", "tcp.options.mptcp.rawdataack", FT_UINT64,
            BASE_DEC, NULL, 0x0, NULL, HFILL}},

        { &hf_tcp_option_mptcp_data_seq_no_raw,
          { "Data Sequence Number", "tcp.options.mptcp.rawdataseqno", FT_UINT64,
            BASE_DEC, NULL, 0x0, NULL, HFILL}},

        { &hf_tcp_option_mptcp_subflow_seq_no,
          { "Subflow Sequence Number", "tcp.options.mptcp.subflowseqno", FT_UINT32,
            BASE_DEC, NULL, 0x0, NULL, HFILL}},

        { &hf_tcp_option_mptcp_data_lvl_len,
          { "Data-level Length", "tcp.options.mptcp.datalvllen", FT_UINT16,
            BASE_DEC, NULL, 0x0, NULL, HFILL}},

        { &hf_tcp_option_mptcp_checksum,
          { "Checksum", "tcp.options.mptcp.checksum", FT_UINT16,
            BASE_HEX, NULL, 0x0, NULL, HFILL}},

        { &hf_tcp_option_mptcp_ipver,
          { "IP version", "tcp.options.mptcp.ipver", FT_UINT8,
            BASE_DEC, NULL, 0x0F, NULL, HFILL}},

        { &hf_tcp_option_mptcp_ipv4,
          { "Advertised IPv4 Address", "tcp.options.mptcp.ipv4", FT_IPv4,
            BASE_NONE, NULL, 0x0, NULL, HFILL}},

        { &hf_tcp_option_mptcp_ipv6,
          { "Advertised IPv6 Address", "tcp.options.mptcp.ipv6", FT_IPv6,
            BASE_NONE, NULL, 0x0, NULL, HFILL}},

        { &hf_tcp_option_mptcp_port,
          { "Advertised port", "tcp.options.mptcp.port", FT_UINT16,
            BASE_DEC, NULL, 0x0, NULL, HFILL}},

        { &hf_tcp_option_cc,
          { "TCP CC Option", "tcp.options.cc_value", FT_UINT32, BASE_DEC,
            NULL, 0x0, NULL, HFILL}},

        { &hf_tcp_option_md5_digest,
          { "MD5 digest", "tcp.options.md5.digest", FT_BYTES, BASE_NONE,
            NULL, 0x0, NULL, HFILL}},

        { &hf_tcp_option_qs_rate,
          { "QS Rate", "tcp.options.qs.rate", FT_UINT8, BASE_DEC|BASE_EXT_STRING,
            &qs_rate_vals_ext, 0x0F, NULL, HFILL}},

        { &hf_tcp_option_qs_ttl_diff,
          { "QS Rate", "tcp.options.qs.ttl_diff", FT_UINT8, BASE_DEC,
            NULL, 0x0, NULL, HFILL}},

        { &hf_tcp_option_scps_vector,
          { "TCP SCPS Capabilities Vector", "tcp.options.scps.vector",
            FT_UINT8, BASE_HEX, NULL, 0x0,
            NULL, HFILL}},

        { &hf_tcp_option_scps_binding,
          { "Binding Space (Community) ID",
            "tcp.options.scps.binding.id",
            FT_UINT8, BASE_DEC, NULL, 0x0,
            "TCP SCPS Extended Binding Space (Community) ID", HFILL}},

        { &hf_tcp_option_scps_binding_len,
          { "Extended Capability Length",
            "tcp.options.scps.binding.len",
            FT_UINT8, BASE_DEC, NULL, 0x0,
            "TCP SCPS Extended Capability Length in bytes", HFILL}},

        { &hf_tcp_option_snack_offset,
          { "TCP SNACK Offset", "tcp.options.snack.offset",
            FT_UINT16, BASE_DEC, NULL, 0x0,
            NULL, HFILL}},

        { &hf_tcp_option_snack_size,
          { "TCP SNACK Size", "tcp.options.snack.size",
            FT_UINT16, BASE_DEC, NULL, 0x0,
            NULL, HFILL}},

        { &hf_tcp_option_snack_le,
          { "TCP SNACK Left Edge", "tcp.options.snack.le",
            FT_UINT16, BASE_DEC, NULL, 0x0,
            NULL, HFILL}},

        { &hf_tcp_option_snack_re,
          { "TCP SNACK Right Edge", "tcp.options.snack.re",
            FT_UINT16, BASE_DEC, NULL, 0x0,
            NULL, HFILL}},

        { &hf_tcp_scpsoption_flags_bets,
          { "Partial Reliability Capable (BETS)",
            "tcp.options.scpsflags.bets", FT_BOOLEAN, 8,
            TFS(&tfs_set_notset), 0x80, NULL, HFILL }},

        { &hf_tcp_scpsoption_flags_snack1,
          { "Short Form SNACK Capable (SNACK1)",
            "tcp.options.scpsflags.snack1", FT_BOOLEAN, 8,
            TFS(&tfs_set_notset), 0x40, NULL, HFILL }},

        { &hf_tcp_scpsoption_flags_snack2,
          { "Long Form SNACK Capable (SNACK2)",
            "tcp.options.scpsflags.snack2", FT_BOOLEAN, 8,
            TFS(&tfs_set_notset), 0x20, NULL, HFILL }},

        { &hf_tcp_scpsoption_flags_compress,
          { "Lossless Header Compression (COMP)",
            "tcp.options.scpsflags.compress", FT_BOOLEAN, 8,
            TFS(&tfs_set_notset), 0x10, NULL, HFILL }},

        { &hf_tcp_scpsoption_flags_nlts,
          { "Network Layer Timestamp (NLTS)",
            "tcp.options.scpsflags.nlts", FT_BOOLEAN, 8,
            TFS(&tfs_set_notset), 0x8, NULL, HFILL }},

        { &hf_tcp_scpsoption_flags_reserved,
          { "Reserved",
            "tcp.options.scpsflags.reserved", FT_UINT8, BASE_DEC,
            NULL, 0x7, NULL, HFILL }},

        { &hf_tcp_scpsoption_connection_id,
          { "Connection ID",
            "tcp.options.scps.binding",
            FT_UINT8, BASE_DEC, NULL, 0x0,
            "TCP SCPS Connection ID", HFILL}},

        { &hf_tcp_option_user_to_granularity,
          { "Granularity", "tcp.options.user_to_granularity", FT_BOOLEAN,
            16, TFS(&tcp_option_user_to_granularity), 0x8000, "TCP User Timeout Granularity", HFILL}},

        { &hf_tcp_option_user_to_val,
          { "User Timeout", "tcp.options.user_to_val", FT_UINT16,
            BASE_DEC, NULL, 0x7FFF, "TCP User Timeout Value", HFILL}},

        { &hf_tcp_option_rvbd_probe_type1,
          { "Type", "tcp.options.rvbd.probe.type1",
            FT_UINT8, BASE_DEC, NULL, 0xF0, NULL, HFILL }},

        { &hf_tcp_option_rvbd_probe_type2,
          { "Type", "tcp.options.rvbd.probe.type2",
            FT_UINT8, BASE_DEC, NULL, 0xFE, NULL, HFILL }},

        { &hf_tcp_option_rvbd_probe_version1,
          { "Version", "tcp.options.rvbd.probe.version",
            FT_UINT8, BASE_DEC, NULL, 0x0F, NULL, HFILL }},

        { &hf_tcp_option_rvbd_probe_version2,
          { "Version", "tcp.options.rvbd.probe.version_raw",
            FT_UINT8, BASE_DEC, NULL, 0x01, "Version 2 Raw Value", HFILL }},

        { &hf_tcp_option_rvbd_probe_prober,
          { "CSH IP", "tcp.options.rvbd.probe.prober",
            FT_IPv4, BASE_NONE, NULL, 0x0, NULL, HFILL }},

        { &hf_tcp_option_rvbd_probe_proxy,
          { "SSH IP", "tcp.options.rvbd.probe.proxy.ip",
            FT_IPv4, BASE_NONE, NULL, 0x0, NULL, HFILL }},

        { &hf_tcp_option_rvbd_probe_proxy_port,
          { "SSH Port", "tcp.options.rvbd.probe.proxy.port",
            FT_UINT16, BASE_DEC, NULL, 0x0, NULL, HFILL }},

        { &hf_tcp_option_rvbd_probe_appli_ver,
          { "Application Version", "tcp.options.rvbd.probe.appli_ver",
            FT_UINT16, BASE_DEC, NULL, 0x0, NULL, HFILL }},

        { &hf_tcp_option_rvbd_probe_client,
          { "Client IP", "tcp.options.rvbd.probe.client.ip",
            FT_IPv4, BASE_NONE, NULL, 0x0, NULL, HFILL }},

        { &hf_tcp_option_rvbd_probe_storeid,
          { "CFE Store ID", "tcp.options.rvbd.probe.storeid",
            FT_UINT32, BASE_DEC, NULL, 0x0, NULL, HFILL }},

        { &hf_tcp_option_rvbd_probe_flags,
          { "Probe Flags", "tcp.options.rvbd.probe.flags",
            FT_UINT8, BASE_HEX, NULL, 0x0, NULL, HFILL }},

        { &hf_tcp_option_rvbd_probe_flag_not_cfe,
          { "Not CFE", "tcp.options.rvbd.probe.flags.notcfe",
            FT_BOOLEAN, 8, TFS(&tfs_set_notset), RVBD_FLAGS_PROBE_NCFE,
            NULL, HFILL }},

        { &hf_tcp_option_rvbd_probe_flag_last_notify,
          { "Last Notify", "tcp.options.rvbd.probe.flags.last",
            FT_BOOLEAN, 8, TFS(&tfs_set_notset), RVBD_FLAGS_PROBE_LAST,
            NULL, HFILL }},

        { &hf_tcp_option_rvbd_probe_flag_probe_cache,
          { "Disable Probe Cache on CSH", "tcp.options.rvbd.probe.flags.probe",
            FT_BOOLEAN, 8, TFS(&tfs_set_notset), RVBD_FLAGS_PROBE,
            NULL, HFILL }},

        { &hf_tcp_option_rvbd_probe_flag_sslcert,
          { "SSL Enabled", "tcp.options.rvbd.probe.flags.ssl",
            FT_BOOLEAN, 8, TFS(&tfs_set_notset), RVBD_FLAGS_PROBE_SSLCERT,
            NULL, HFILL }},

        { &hf_tcp_option_rvbd_probe_flag_server_connected,
          { "SSH outer to server established", "tcp.options.rvbd.probe.flags.server",
            FT_BOOLEAN, 8, TFS(&tfs_set_notset), RVBD_FLAGS_PROBE_SERVER,
            NULL, HFILL }},

        { &hf_tcp_option_rvbd_trpy_flags,
          { "Transparency Options", "tcp.options.rvbd.trpy.flags",
            FT_UINT16, BASE_HEX, NULL, 0x0, NULL, HFILL }},

        { &hf_tcp_option_rvbd_trpy_flag_fw_rst_probe,
          { "Enable FW traversal feature", "tcp.options.rvbd.trpy.flags.fw_rst_probe",
            FT_BOOLEAN, 16, TFS(&tfs_set_notset),
            RVBD_FLAGS_TRPY_FW_RST_PROBE,
            "Reset state created by probe on the nexthop firewall",
            HFILL }},

        { &hf_tcp_option_rvbd_trpy_flag_fw_rst_inner,
          { "Enable Inner FW feature on All FWs", "tcp.options.rvbd.trpy.flags.fw_rst_inner",
            FT_BOOLEAN, 16, TFS(&tfs_set_notset),
            RVBD_FLAGS_TRPY_FW_RST_INNER,
            "Reset state created by transparent inner on all firewalls"
            " before passing connection through",
            HFILL }},

        { &hf_tcp_option_rvbd_trpy_flag_fw_rst,
          { "Enable Transparency FW feature on All FWs", "tcp.options.rvbd.trpy.flags.fw_rst",
            FT_BOOLEAN, 16, TFS(&tfs_set_notset),
            RVBD_FLAGS_TRPY_FW_RST,
            "Reset state created by probe on all firewalls before "
            "establishing transparent inner connection", HFILL }},

        { &hf_tcp_option_rvbd_trpy_flag_chksum,
          { "Reserved", "tcp.options.rvbd.trpy.flags.chksum",
            FT_BOOLEAN, 16, TFS(&tfs_set_notset),
            RVBD_FLAGS_TRPY_CHKSUM, NULL, HFILL }},

        { &hf_tcp_option_rvbd_trpy_flag_oob,
          { "Out of band connection", "tcp.options.rvbd.trpy.flags.oob",
            FT_BOOLEAN, 16, TFS(&tfs_set_notset),
            RVBD_FLAGS_TRPY_OOB, NULL, HFILL }},

        { &hf_tcp_option_rvbd_trpy_flag_mode,
          { "Transparency Mode", "tcp.options.rvbd.trpy.flags.mode",
            FT_BOOLEAN, 16, TFS(&trpy_mode_str),
            RVBD_FLAGS_TRPY_MODE, NULL, HFILL }},

        { &hf_tcp_option_rvbd_trpy_src,
          { "Src SH IP Addr", "tcp.options.rvbd.trpy.src.ip",
            FT_IPv4, BASE_NONE, NULL, 0x0, NULL, HFILL }},

        { &hf_tcp_option_rvbd_trpy_dst,
          { "Dst SH IP Addr", "tcp.options.rvbd.trpy.dst.ip",
            FT_IPv4, BASE_NONE, NULL, 0x0, NULL, HFILL }},

        { &hf_tcp_option_rvbd_trpy_src_port,
          { "Src SH Inner Port", "tcp.options.rvbd.trpy.src.port",
            FT_UINT16, BASE_DEC, NULL, 0x0, NULL, HFILL }},

        { &hf_tcp_option_rvbd_trpy_dst_port,
          { "Dst SH Inner Port", "tcp.options.rvbd.trpy.dst.port",
            FT_UINT16, BASE_DEC, NULL, 0x0, NULL, HFILL }},

        { &hf_tcp_option_rvbd_trpy_client_port,
          { "Out of band connection Client Port", "tcp.options.rvbd.trpy.client.port",
            FT_UINT16, BASE_DEC, NULL , 0x0, NULL, HFILL }},

        { &hf_tcp_option_fast_open_cookie_request,
          { "Fast Open Cookie Request", "tcp.options.tfo.request", FT_NONE,
            BASE_NONE, NULL, 0x0, NULL, HFILL }},

        { &hf_tcp_option_fast_open_cookie,
          { "Fast Open Cookie", "tcp.options.tfo.cookie", FT_BYTES,
            BASE_NONE, NULL, 0x0, NULL, HFILL}},

        { &hf_tcp_pdu_time,
          { "Time until the last segment of this PDU", "tcp.pdu.time", FT_RELATIVE_TIME, BASE_NONE, NULL, 0x0,
            "How long time has passed until the last frame of this PDU", HFILL}},

        { &hf_tcp_pdu_size,
          { "PDU Size", "tcp.pdu.size", FT_UINT32, BASE_DEC, NULL, 0x0,
            "The size of this PDU", HFILL}},

        { &hf_tcp_pdu_last_frame,
          { "Last frame of this PDU", "tcp.pdu.last_frame", FT_FRAMENUM, BASE_NONE, NULL, 0x0,
            "This is the last frame of the PDU starting in this segment", HFILL }},

        { &hf_tcp_ts_relative,
          { "Time since first frame in this TCP stream", "tcp.time_relative", FT_RELATIVE_TIME, BASE_NONE, NULL, 0x0,
            "Time relative to first frame in this TCP stream", HFILL}},

        { &hf_tcp_ts_delta,
          { "Time since previous frame in this TCP stream", "tcp.time_delta", FT_RELATIVE_TIME, BASE_NONE, NULL, 0x0,
            "Time delta from previous frame in this TCP stream", HFILL}},

        { &hf_tcp_proc_src_uid,
          { "Source process user ID", "tcp.proc.srcuid", FT_UINT32, BASE_DEC, NULL, 0x0,
            NULL, HFILL}},

        { &hf_tcp_proc_src_pid,
          { "Source process ID", "tcp.proc.srcpid", FT_UINT32, BASE_DEC, NULL, 0x0,
            NULL, HFILL}},

        { &hf_tcp_proc_src_uname,
          { "Source process user name", "tcp.proc.srcuname", FT_STRING, BASE_NONE, NULL, 0x0,
            NULL, HFILL}},

        { &hf_tcp_proc_src_cmd,
          { "Source process name", "tcp.proc.srccmd", FT_STRING, BASE_NONE, NULL, 0x0,
            "Source process command name", HFILL}},

        { &hf_tcp_proc_dst_uid,
          { "Destination process user ID", "tcp.proc.dstuid", FT_UINT32, BASE_DEC, NULL, 0x0,
            NULL, HFILL}},

        { &hf_tcp_proc_dst_pid,
          { "Destination process ID", "tcp.proc.dstpid", FT_UINT32, BASE_DEC, NULL, 0x0,
            NULL, HFILL}},

        { &hf_tcp_proc_dst_uname,
          { "Destination process user name", "tcp.proc.dstuname", FT_STRING, BASE_NONE, NULL, 0x0,
            NULL, HFILL}},

        { &hf_tcp_proc_dst_cmd,
          { "Destination process name", "tcp.proc.dstcmd", FT_STRING, BASE_NONE, NULL, 0x0,
            "Destination process command name", HFILL}},

        { &hf_tcp_segment_data,
          { "TCP segment data", "tcp.segment_data", FT_BYTES, BASE_NONE, NULL, 0x0,
            "A data segment used in reassembly of a lower-level protocol", HFILL}},

        { &hf_tcp_payload,
          { "TCP payload", "tcp.payload", FT_BYTES, BASE_NONE, NULL, 0x0,
            "The TCP payload of this packet", HFILL}},

        { &hf_tcp_option_scps_binding_data,
          { "Binding Space Data", "tcp.options.scps.binding.data", FT_BYTES, BASE_NONE, NULL, 0x0,
            NULL, HFILL }},

        { &hf_tcp_option_rvbd_probe_reserved,
          { "Reserved", "tcp.options.rvbd.probe.reserved", FT_UINT8, BASE_HEX, NULL, 0x0,
            NULL, HFILL }},

        { &hf_tcp_fin_retransmission,
          { "Retransmission of FIN from frame", "tcp.fin_retransmission", FT_FRAMENUM, BASE_NONE, NULL, 0x0,
            NULL, HFILL }},

        { &hf_tcp_reset_cause,
          { "Reset cause", "tcp.reset_cause", FT_STRING, BASE_NONE, NULL, 0x0,
            NULL, HFILL }},
    };

    static gint *ett[] = {
        &ett_tcp,
        &ett_tcp_flags,
        &ett_tcp_options,
        &ett_tcp_option_timestamp,
        &ett_tcp_option_mptcp,
        &ett_tcp_option_wscale,
        &ett_tcp_option_sack,
        &ett_tcp_option_snack,
        &ett_tcp_option_scps,
        &ett_tcp_scpsoption_flags,
        &ett_tcp_option_scps_extended,
        &ett_tcp_option_user_to,
        &ett_tcp_option_exp,
        &ett_tcp_option_sack_perm,
        &ett_tcp_option_mss,
        &ett_tcp_opt_rvbd_probe,
        &ett_tcp_opt_rvbd_probe_flags,
        &ett_tcp_opt_rvbd_trpy,
        &ett_tcp_opt_rvbd_trpy_flags,
        &ett_tcp_opt_echo,
        &ett_tcp_opt_cc,
        &ett_tcp_opt_md5,
        &ett_tcp_opt_qs,
        &ett_tcp_analysis_faults,
        &ett_tcp_analysis,
        &ett_tcp_timestamps,
        &ett_tcp_segments,
        &ett_tcp_segment,
        &ett_tcp_checksum,
        &ett_tcp_process_info,
        &ett_tcp_unknown_opt,
        &ett_tcp_opt_recbound,
        &ett_tcp_opt_scpscor,
        &ett_tcp_option_other
    };

    static gint *mptcp_ett[] = {
        &ett_mptcp_analysis,
        &ett_mptcp_analysis_subflows
    };

    static const enum_val_t window_scaling_vals[] = {
        {"not-known",  "Not known",                  WindowScaling_NotKnown},
        {"0",          "0 (no scaling)",             WindowScaling_0},
        {"1",          "1 (multiply by 2)",          WindowScaling_1},
        {"2",          "2 (multiply by 4)",          WindowScaling_2},
        {"3",          "3 (multiply by 8)",          WindowScaling_3},
        {"4",          "4 (multiply by 16)",         WindowScaling_4},
        {"5",          "5 (multiply by 32)",         WindowScaling_5},
        {"6",          "6 (multiply by 64)",         WindowScaling_6},
        {"7",          "7 (multiply by 128)",        WindowScaling_7},
        {"8",          "8 (multiply by 256)",        WindowScaling_8},
        {"9",          "9 (multiply by 512)",        WindowScaling_9},
        {"10",         "10 (multiply by 1024)",      WindowScaling_10},
        {"11",         "11 (multiply by 2048)",      WindowScaling_11},
        {"12",         "12 (multiply by 4096)",      WindowScaling_12},
        {"13",         "13 (multiply by 8192)",      WindowScaling_13},
        {"14",         "14 (multiply by 16384)",     WindowScaling_14},
        {NULL, NULL, -1}
    };

    static ei_register_info ei[] = {
        { &ei_tcp_opt_len_invalid, { "tcp.option.len.invalid", PI_SEQUENCE, PI_NOTE, "Invalid length for option", EXPFILL }},
        { &ei_tcp_analysis_retransmission, { "tcp.analysis.retransmission", PI_SEQUENCE, PI_NOTE, "This frame is a (suspected) retransmission", EXPFILL }},
        { &ei_tcp_analysis_fast_retransmission, { "tcp.analysis.fast_retransmission", PI_SEQUENCE, PI_NOTE, "This frame is a (suspected) fast retransmission", EXPFILL }},
        { &ei_tcp_analysis_spurious_retransmission, { "tcp.analysis.spurious_retransmission", PI_SEQUENCE, PI_NOTE, "This frame is a (suspected) spurious retransmission", EXPFILL }},
        { &ei_tcp_analysis_out_of_order, { "tcp.analysis.out_of_order", PI_SEQUENCE, PI_WARN, "This frame is a (suspected) out-of-order segment", EXPFILL }},
        { &ei_tcp_analysis_reused_ports, { "tcp.analysis.reused_ports", PI_SEQUENCE, PI_NOTE, "A new tcp session is started with the same ports as an earlier session in this trace", EXPFILL }},
        { &ei_tcp_analysis_lost_packet, { "tcp.analysis.lost_segment", PI_SEQUENCE, PI_WARN, "Previous segment(s) not captured (common at capture start)", EXPFILL }},
        { &ei_tcp_analysis_ack_lost_packet, { "tcp.analysis.ack_lost_segment", PI_SEQUENCE, PI_WARN, "ACKed segment that wasn't captured (common at capture start)", EXPFILL }},
        { &ei_tcp_analysis_window_update, { "tcp.analysis.window_update", PI_SEQUENCE, PI_CHAT, "TCP window update", EXPFILL }},
        { &ei_tcp_analysis_window_full, { "tcp.analysis.window_full", PI_SEQUENCE, PI_WARN, "TCP window specified by the receiver is now completely full", EXPFILL }},
        { &ei_tcp_analysis_keep_alive, { "tcp.analysis.keep_alive", PI_SEQUENCE, PI_NOTE, "TCP keep-alive segment", EXPFILL }},
        { &ei_tcp_analysis_keep_alive_ack, { "tcp.analysis.keep_alive_ack", PI_SEQUENCE, PI_NOTE, "ACK to a TCP keep-alive segment", EXPFILL }},
        { &ei_tcp_analysis_duplicate_ack, { "tcp.analysis.duplicate_ack", PI_SEQUENCE, PI_NOTE, "Duplicate ACK", EXPFILL }},
        { &ei_tcp_analysis_zero_window_probe, { "tcp.analysis.zero_window_probe", PI_SEQUENCE, PI_NOTE, "TCP Zero Window Probe", EXPFILL }},
        { &ei_tcp_analysis_zero_window, { "tcp.analysis.zero_window", PI_SEQUENCE, PI_WARN, "TCP Zero Window segment", EXPFILL }},
        { &ei_tcp_analysis_zero_window_probe_ack, { "tcp.analysis.zero_window_probe_ack", PI_SEQUENCE, PI_NOTE, "ACK to a TCP Zero Window Probe", EXPFILL }},
        { &ei_tcp_analysis_tfo_syn, { "tcp.analysis.tfo_syn", PI_SEQUENCE, PI_NOTE, "TCP SYN with TFO Cookie", EXPFILL }},
        { &ei_tcp_scps_capable, { "tcp.analysis.zero_window_probe_ack", PI_SEQUENCE, PI_NOTE, "Connection establish request (SYN-ACK): SCPS Capabilities Negotiated", EXPFILL }},
        { &ei_tcp_option_snack_sequence, { "tcp.options.snack.sequence", PI_SEQUENCE, PI_NOTE, "SNACK Sequence", EXPFILL }},
        { &ei_tcp_option_wscale_shift_invalid, { "tcp.options.wscale.shift.invalid", PI_PROTOCOL, PI_WARN, "Window scale shift exceeds 14", EXPFILL }},
        { &ei_tcp_short_segment, { "tcp.short_segment", PI_MALFORMED, PI_WARN, "Short segment", EXPFILL }},
        { &ei_tcp_ack_nonzero, { "tcp.ack.nonzero", PI_PROTOCOL, PI_NOTE, "The acknowledgment number field is nonzero while the ACK flag is not set", EXPFILL }},
        { &ei_tcp_connection_sack, { "tcp.connection.sack", PI_SEQUENCE, PI_CHAT, "Connection establish acknowledge (SYN+ACK)", EXPFILL }},
        { &ei_tcp_connection_syn, { "tcp.connection.syn", PI_SEQUENCE, PI_CHAT, "Connection establish request (SYN)", EXPFILL }},
        { &ei_tcp_connection_fin, { "tcp.connection.fin", PI_SEQUENCE, PI_CHAT, "Connection finish (FIN)", EXPFILL }},
        /* According to RFCs, RST is an indication of an error. Some applications use it
         * to terminate a connection as well, which is a misbehavior (see e.g. rfc3360)
         */
        { &ei_tcp_connection_rst, { "tcp.connection.rst", PI_SEQUENCE, PI_WARN, "Connection reset (RST)", EXPFILL }},
        { &ei_tcp_checksum_ffff, { "tcp.checksum.ffff", PI_CHECKSUM, PI_WARN, "TCP Checksum 0xffff instead of 0x0000 (see RFC 1624)", EXPFILL }},
        { &ei_tcp_checksum_bad, { "tcp.checksum_bad.expert", PI_CHECKSUM, PI_ERROR, "Bad checksum", EXPFILL }},
        { &ei_tcp_urgent_pointer_non_zero, { "tcp.urgent_pointer.non_zero", PI_PROTOCOL, PI_NOTE, "The urgent pointer field is nonzero while the URG flag is not set", EXPFILL }},
        { &ei_tcp_suboption_malformed, { "tcp.suboption_malformed", PI_MALFORMED, PI_ERROR, "suboption would go past end of option", EXPFILL }},
        { &ei_tcp_nop, { "tcp.nop", PI_PROTOCOL, PI_WARN, "4 NOP in a row - a router may have removed some options", EXPFILL }},
        { &ei_tcp_bogus_header_length, { "tcp.bogus_header_length", PI_PROTOCOL, PI_ERROR, "Bogus TCP Header length", EXPFILL }},
    };

    static ei_register_info mptcp_ei[] = {
#if 0
        { &ei_mptcp_analysis_unexpected_idsn, { "mptcp.connection.unexpected_idsn", PI_PROTOCOL, PI_NOTE, "Unexpected initial sequence number", EXPFILL }},
#endif
        { &ei_mptcp_analysis_echoed_key_mismatch, { "mptcp.connection.echoed_key_mismatch", PI_PROTOCOL, PI_WARN, "The echoed key in the ACK of the MPTCP handshake does not match the key of the SYN/ACK", EXPFILL }},
        { &ei_mptcp_analysis_missing_algorithm, { "mptcp.connection.missing_algorithm", PI_PROTOCOL, PI_WARN, "No crypto algorithm specified", EXPFILL }},
        { &ei_mptcp_analysis_unsupported_algorithm, { "mptcp.connection.unsupported_algorithm", PI_PROTOCOL, PI_WARN, "Unsupported algorithm", EXPFILL }},
        { &ei_mptcp_infinite_mapping, { "mptcp.dss.infinite_mapping", PI_PROTOCOL, PI_WARN, "Fallback to infinite mapping", EXPFILL }},
        { &ei_mptcp_mapping_missing, { "mptcp.dss.missing_mapping", PI_PROTOCOL, PI_WARN, "No mapping available", EXPFILL }},
#if 0
        { &ei_mptcp_stream_incomplete, { "mptcp.incomplete", PI_PROTOCOL, PI_WARN, "Everything was not captured", EXPFILL }},
        { &ei_mptcp_analysis_dsn_out_of_order, { "mptcp.analysis.dsn.out_of_order", PI_PROTOCOL, PI_WARN, "Out of order dsn", EXPFILL }},
#endif
    };

    static hf_register_info mptcp_hf[] = {
        { &hf_mptcp_ack,
          { "Multipath TCP Data ACK", "mptcp.ack", FT_UINT64,
            BASE_DEC, NULL, 0x0, NULL, HFILL}},

        { &hf_mptcp_dsn,
          { "Data Sequence Number", "mptcp.dsn", FT_UINT64, BASE_DEC, NULL, 0x0,
            "Data Sequence Number mapped to this TCP sequence number", HFILL}},

        { &hf_mptcp_rawdsn64,
          { "Raw Data Sequence Number", "mptcp.rawdsn64", FT_UINT64, BASE_DEC, NULL, 0x0,
            "Data Sequence Number mapped to this TCP sequence number", HFILL}},

        { &hf_mptcp_dss_dsn,
          { "DSS Data Sequence Number", "mptcp.dss.dsn", FT_UINT64,
            BASE_DEC, NULL, 0x0, NULL, HFILL}},

        { &hf_mptcp_expected_idsn,
          { "Subflow expected IDSN", "mptcp.expected_idsn", FT_UINT64,
            BASE_DEC|BASE_UNIT_STRING, &units_64bit_version, 0x0, NULL, HFILL}},

        { &hf_mptcp_analysis_subflows_stream_id,
          { "List subflow Stream IDs", "mptcp.analysis.subflows.streamid", FT_UINT16,
            BASE_DEC, NULL, 0x0, NULL, HFILL}},

        { &hf_mptcp_analysis,
          { "MPTCP analysis",   "mptcp.analysis", FT_NONE, BASE_NONE, NULL, 0x0,
            "This frame has some of the MPTCP analysis shown", HFILL }},

        { &hf_mptcp_related_mapping,
          { "Related mapping", "mptcp.related_mapping", FT_FRAMENUM , BASE_NONE, NULL, 0x0,
            "Packet in which current packet DSS mapping was sent", HFILL }},

        { &hf_mptcp_reinjection_of,
          { "Reinjection of", "mptcp.reinjection_of", FT_FRAMENUM , BASE_NONE, NULL, 0x0,
            "This is a retransmission of data sent on another subflow", HFILL }},

        { &hf_mptcp_reinjected_in,
          { "Data reinjected in", "mptcp.reinjected_in", FT_FRAMENUM , BASE_NONE, NULL, 0x0,
            "This was retransmitted on another subflow", HFILL }},

        { &hf_mptcp_analysis_subflows,
          { "TCP subflow stream id(s):", "mptcp.analysis.subflows", FT_NONE, BASE_NONE, NULL, 0x0,
            "List all TCP connections mapped to this MPTCP connection", HFILL }},

        { &hf_mptcp_stream,
          { "Stream index", "mptcp.stream", FT_UINT32, BASE_DEC, NULL, 0x0,
            NULL, HFILL }},

        { &hf_mptcp_number_of_removed_addresses,
          { "Number of removed addresses", "mptcp.rm_addr.count", FT_UINT8,
            BASE_DEC, NULL, 0x0, NULL, HFILL}},

        { &hf_mptcp_expected_token,
          { "Subflow token generated from key", "mptcp.expected_token", FT_UINT32,
            BASE_DEC, NULL, 0x0, NULL, HFILL}},

        { &hf_mptcp_analysis_master,
          { "Master flow", "mptcp.master", FT_BOOLEAN, BASE_NONE,
            NULL, 0x0, NULL, HFILL}}

    };

    static build_valid_func tcp_da_src_values[1] = {tcp_src_value};
    static build_valid_func tcp_da_dst_values[1] = {tcp_dst_value};
    static build_valid_func tcp_da_both_values[2] = {tcp_src_value, tcp_dst_value};
    static decode_as_value_t tcp_da_values[3] = {{tcp_src_prompt, 1, tcp_da_src_values}, {tcp_dst_prompt, 1, tcp_da_dst_values}, {tcp_both_prompt, 2, tcp_da_both_values}};
    static decode_as_t tcp_da = {"tcp", "Transport", "tcp.port", 3, 2, tcp_da_values, "TCP", "port(s) as",
                                 decode_as_default_populate_list, decode_as_default_reset, decode_as_default_change, NULL};

    module_t *tcp_module;
    module_t *mptcp_module;
    expert_module_t* expert_tcp;
    expert_module_t* expert_mptcp;

    proto_tcp = proto_register_protocol("Transmission Control Protocol", "TCP", "tcp");
    tcp_handle = register_dissector("tcp", dissect_tcp, proto_tcp);
    proto_register_field_array(proto_tcp, hf, array_length(hf));
    proto_register_subtree_array(ett, array_length(ett));
    expert_tcp = expert_register_protocol(proto_tcp);
    expert_register_field_array(expert_tcp, ei, array_length(ei));

    /* subdissector code */
    subdissector_table = register_dissector_table("tcp.port",
        "TCP port", proto_tcp, FT_UINT16, BASE_DEC);
    heur_subdissector_list = register_heur_dissector_list("tcp", proto_tcp);
    tcp_option_table = register_dissector_table("tcp.option",
        "TCP Options", proto_tcp, FT_UINT8, BASE_DEC);

    /* Register TCP options as their own protocols so we can get the name of the option */
    proto_tcp_option_nop = proto_register_protocol_in_name_only("TCP Option - No-Operation (NOP)", "No-Operation (NOP)", "tcp.options.nop", proto_tcp, FT_BYTES);
    proto_tcp_option_eol = proto_register_protocol_in_name_only("TCP Option - End of Option List (EOL)", "End of Option List (EOL)", "tcp.options.eol", proto_tcp, FT_BYTES);
    proto_tcp_option_timestamp = proto_register_protocol_in_name_only("TCP Option - Timestamps", "Timestamps", "tcp.options.timestamp", proto_tcp, FT_BYTES);
    proto_tcp_option_mss = proto_register_protocol_in_name_only("TCP Option - Maximum segment size", "Maximum segment size", "tcp.options.mss", proto_tcp, FT_BYTES);
    proto_tcp_option_wscale = proto_register_protocol_in_name_only("TCP Option - Window scale", "Window scale", "tcp.options.wscale", proto_tcp, FT_BYTES);
    proto_tcp_option_sack_perm = proto_register_protocol_in_name_only("TCP Option - SACK permitted", "SACK permitted", "tcp.options.sack_perm", proto_tcp, FT_BYTES);
    proto_tcp_option_sack = proto_register_protocol_in_name_only("TCP Option - SACK", "SACK", "tcp.options.sack", proto_tcp, FT_BYTES);
    proto_tcp_option_echo = proto_register_protocol_in_name_only("TCP Option - Echo", "Echo", "tcp.options.echo", proto_tcp, FT_BYTES);
    proto_tcp_option_echoreply = proto_register_protocol_in_name_only("TCP Option - Echo reply", "Echo reply", "tcp.options.echoreply", proto_tcp, FT_BYTES);
    proto_tcp_option_cc = proto_register_protocol_in_name_only("TCP Option - CC", "CC", "tcp.options.cc", proto_tcp, FT_BYTES);
    proto_tcp_option_cc_new = proto_register_protocol_in_name_only("TCP Option - CC.NEW", "CC.NEW", "tcp.options.ccnew", proto_tcp, FT_BYTES);
    proto_tcp_option_cc_echo = proto_register_protocol_in_name_only("TCP Option - CC.ECHO", "CC.ECHO", "tcp.options.ccecho", proto_tcp, FT_BYTES);
    proto_tcp_option_md5 = proto_register_protocol_in_name_only("TCP Option - TCP MD5 signature", "TCP MD5 signature", "tcp.options.md5", proto_tcp, FT_BYTES);
    proto_tcp_option_scps = proto_register_protocol_in_name_only("TCP Option - SCPS capabilities", "SCPS capabilities", "tcp.options.scps", proto_tcp, FT_BYTES);
    proto_tcp_option_snack = proto_register_protocol_in_name_only("TCP Option - Selective Negative Acknowledgment", "Selective Negative Acknowledgment", "tcp.options.snack", proto_tcp, FT_BYTES);
    proto_tcp_option_scpsrec = proto_register_protocol_in_name_only("TCP Option - SCPS record boundary", "SCPS record boundary", "tcp.options.scpsrec", proto_tcp, FT_BYTES);
    proto_tcp_option_scpscor = proto_register_protocol_in_name_only("TCP Option - SCPS corruption experienced", "SCPS corruption experienced", "tcp.options.scpscor", proto_tcp, FT_BYTES);
    proto_tcp_option_qs = proto_register_protocol_in_name_only("TCP Option - Quick-Start", "Quick-Start", "tcp.options.qs", proto_tcp, FT_BYTES);
    proto_tcp_option_user_to = proto_register_protocol_in_name_only("TCP Option - User Timeout", "User Timeout", "tcp.options.user_to", proto_tcp, FT_BYTES);
    proto_tcp_option_tfo = proto_register_protocol_in_name_only("TCP Option - TCP Fast Open", "TCP Fast Open", "tcp.options.tfo", proto_tcp, FT_BYTES);
    proto_tcp_option_rvbd_probe = proto_register_protocol_in_name_only("TCP Option - Riverbed Probe", "Riverbed Probe", "tcp.options.rvbd.probe", proto_tcp, FT_BYTES);
    proto_tcp_option_rvbd_trpy = proto_register_protocol_in_name_only("TCP Option - Riverbed Transparency", "Riverbed Transparency", "tcp.options.rvbd.trpy", proto_tcp, FT_BYTES);
    proto_tcp_option_exp = proto_register_protocol_in_name_only("TCP Option - Experimental", "Experimental", "tcp.options.experimental", proto_tcp, FT_BYTES);
    proto_tcp_option_unknown = proto_register_protocol_in_name_only("TCP Option - Unknown", "Unknown", "tcp.options.unknown", proto_tcp, FT_BYTES);

    register_capture_dissector_table("tcp.port", "TCP");

    /* Register configuration preferences */
    tcp_module = prefs_register_protocol(proto_tcp, NULL);
    prefs_register_bool_preference(tcp_module, "summary_in_tree",
        "Show TCP summary in protocol tree",
        "Whether the TCP summary line should be shown in the protocol tree",
        &tcp_summary_in_tree);
    prefs_register_bool_preference(tcp_module, "check_checksum",
        "Validate the TCP checksum if possible",
        "Whether to validate the TCP checksum or not.  "
        "(Invalid checksums will cause reassembly, if enabled, to fail.)",
        &tcp_check_checksum);
    prefs_register_bool_preference(tcp_module, "desegment_tcp_streams",
        "Allow subdissector to reassemble TCP streams",
        "Whether subdissector can request TCP streams to be reassembled",
        &tcp_desegment);
    prefs_register_bool_preference(tcp_module, "reassemble_out_of_order",
        "Reassemble out-of-order segments",
        "Whether out-of-order segments should be buffered and reordered before passing it to a subdissector. "
        "To use this option you must also enable \"Allow subdissector to reassemble TCP streams\".",
        &tcp_reassemble_out_of_order);
    prefs_register_bool_preference(tcp_module, "analyze_sequence_numbers",
        "Analyze TCP sequence numbers",
        "Make the TCP dissector analyze TCP sequence numbers to find and flag segment retransmissions, missing segments and RTT",
        &tcp_analyze_seq);
    prefs_register_bool_preference(tcp_module, "relative_sequence_numbers",
        "Relative sequence numbers (Requires \"Analyze TCP sequence numbers\")",
        "Make the TCP dissector use relative sequence numbers instead of absolute ones. "
        "To use this option you must also enable \"Analyze TCP sequence numbers\". ",
        &tcp_relative_seq);
    prefs_register_enum_preference(tcp_module, "default_window_scaling",
        "Scaling factor to use when not available from capture",
        "Make the TCP dissector use this scaling factor for streams where the signalled scaling factor "
        "is not visible in the capture",
        &tcp_default_window_scaling, window_scaling_vals, FALSE);

    /* Presumably a retired, unconditional version of what has been added back with the preference above... */
    prefs_register_obsolete_preference(tcp_module, "window_scaling");

    prefs_register_bool_preference(tcp_module, "track_bytes_in_flight",
        "Track number of bytes in flight",
        "Make the TCP dissector track the number on un-ACKed bytes of data are in flight per packet. "
        "To use this option you must also enable \"Analyze TCP sequence numbers\". "
        "This takes a lot of memory but allows you to track how much data are in flight at a time and graphing it in io-graphs",
        &tcp_track_bytes_in_flight);
    prefs_register_bool_preference(tcp_module, "calculate_timestamps",
        "Calculate conversation timestamps",
        "Calculate timestamps relative to the first frame and the previous frame in the tcp conversation",
        &tcp_calculate_ts);
    prefs_register_bool_preference(tcp_module, "try_heuristic_first",
        "Try heuristic sub-dissectors first",
        "Try to decode a packet using an heuristic sub-dissector before using a sub-dissector registered to a specific port",
        &try_heuristic_first);
    prefs_register_bool_preference(tcp_module, "ignore_tcp_timestamps",
        "Ignore TCP Timestamps in summary",
        "Do not place the TCP Timestamps in the summary line",
        &tcp_ignore_timestamps);

    prefs_register_bool_preference(tcp_module, "no_subdissector_on_error",
        "Do not call subdissectors for error packets",
        "Do not call any subdissectors for Retransmitted or OutOfOrder segments",
        &tcp_no_subdissector_on_error);

    prefs_register_bool_preference(tcp_module, "dissect_experimental_options_with_magic",
        "TCP Experimental Options with a Magic Number",
        "Assume TCP Experimental Options (253, 254) have a Magic Number and use it for dissection",
        &tcp_exp_options_with_magic);

    prefs_register_bool_preference(tcp_module, "display_process_info_from_ipfix",
        "Display process information via IPFIX",
        "Collect and store process information retrieved from IPFIX dissector",
        &tcp_display_process_info);

    register_init_routine(tcp_init);
    reassembly_table_register(&tcp_reassembly_table,
                          &addresses_ports_reassembly_table_functions);

    register_decode_as(&tcp_da);

    register_conversation_table(proto_tcp, FALSE, tcpip_conversation_packet, tcpip_hostlist_packet);
    register_conversation_filter("tcp", "TCP", tcp_filter_valid, tcp_build_filter);

    register_seq_analysis("tcp", "TCP Flows", proto_tcp, NULL, 0, tcp_seq_analysis_packet);

    /* considers MPTCP as a distinct protocol (even if it's a TCP option) */
    proto_mptcp = proto_register_protocol("Multipath Transmission Control Protocol", "MPTCP", "mptcp");

    proto_register_field_array(proto_mptcp, mptcp_hf, array_length(mptcp_hf));
    proto_register_subtree_array(mptcp_ett, array_length(mptcp_ett));

    /* Register configuration preferences */
    mptcp_module = prefs_register_protocol(proto_mptcp, NULL);
    expert_mptcp = expert_register_protocol(proto_tcp);
    expert_register_field_array(expert_mptcp, mptcp_ei, array_length(mptcp_ei));

    prefs_register_bool_preference(mptcp_module, "analyze_mptcp",
        "Map TCP subflows to their respective MPTCP connections",
        "To use this option you must also enable \"Analyze TCP sequence numbers\". ",
        &tcp_analyze_mptcp);

    prefs_register_bool_preference(mptcp_module, "relative_sequence_numbers",
        "Display relative MPTCP sequence numbers.",
        "In case you don't capture the key, it will use the first DSN seen",
        &mptcp_relative_seq);

    prefs_register_bool_preference(mptcp_module, "analyze_mappings",
        "Deeper analysis of Data Sequence Signal (DSS)",
        "Scales logarithmically with the number of packets"
        "You need to capture the handshake for this to work."
        "\"Map TCP subflows to their respective MPTCP connections\"",
        &mptcp_analyze_mappings);

    prefs_register_bool_preference(mptcp_module, "intersubflows_retransmission",
        "Check for data duplication across subflows",
        "(Greedy algorithm: Scales linearly with number of subflows and"
        " logarithmic scaling with number of packets)"
        "You need to enable DSS mapping analysis for this option to work",
        &mptcp_intersubflows_retransmission);

    register_conversation_table(proto_mptcp, FALSE, mptcpip_conversation_packet, tcpip_hostlist_packet);
    register_follow_stream(proto_tcp, "tcp_follow", tcp_follow_conv_filter, tcp_follow_index_filter, tcp_follow_address_filter,
                            tcp_port_to_display, follow_tcp_tap_listener);
}
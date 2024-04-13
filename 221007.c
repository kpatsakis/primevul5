dissect_dnp3_tcp(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, void *data)
{
  if (!check_dnp3_header(tvb, FALSE)) {
    return 0;
  }

  tcp_dissect_pdus(tvb, pinfo, tree, TRUE, DNP_HDR_LEN,
                   get_dnp3_message_len, dissect_dnp3_message, data);

  return tvb_captured_length(tvb);
}
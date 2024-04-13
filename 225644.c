  StreamIO(CephContext *cct, Stream& stream, rgw::asio::parser_type& parser,
           spawn::yield_context yield,
           parse_buffer& buffer, bool is_ssl,
           const tcp::endpoint& local_endpoint,
           const tcp::endpoint& remote_endpoint)
      : ClientIO(parser, is_ssl, local_endpoint, remote_endpoint),
        cct(cct), stream(stream), yield(yield), buffer(buffer)
  {}
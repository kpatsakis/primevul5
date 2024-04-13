void handle_connection(boost::asio::io_context& context,
                       RGWProcessEnv& env, Stream& stream,
                       parse_buffer& buffer, bool is_ssl,
                       SharedMutex& pause_mutex,
                       rgw::dmclock::Scheduler *scheduler,
                       boost::system::error_code& ec,
                       spawn::yield_context yield)
{
  // limit header to 4k, since we read it all into a single flat_buffer
  static constexpr size_t header_limit = 4096;
  // don't impose a limit on the body, since we read it in pieces
  static constexpr size_t body_limit = std::numeric_limits<size_t>::max();

  auto cct = env.store->ctx();

  // read messages from the stream until eof
  for (;;) {
    // configure the parser
    rgw::asio::parser_type parser;
    parser.header_limit(header_limit);
    parser.body_limit(body_limit);

    // parse the header
    http::async_read_header(stream, buffer, parser, yield[ec]);
    if (ec == boost::asio::error::connection_reset ||
        ec == boost::asio::error::bad_descriptor ||
        ec == boost::asio::error::operation_aborted ||
#ifdef WITH_RADOSGW_BEAST_OPENSSL
        ec == ssl::error::stream_truncated ||
#endif
        ec == http::error::end_of_stream) {
      ldout(cct, 20) << "failed to read header: " << ec.message() << dendl;
      return;
    }
    if (ec) {
      ldout(cct, 1) << "failed to read header: " << ec.message() << dendl;
      auto& message = parser.get();
      http::response<http::empty_body> response;
      response.result(http::status::bad_request);
      response.version(message.version() == 10 ? 10 : 11);
      response.prepare_payload();
      http::async_write(stream, response, yield[ec]);
      if (ec) {
        ldout(cct, 5) << "failed to write response: " << ec.message() << dendl;
      }
      ldout(cct, 1) << "====== req done http_status=400 ======" << dendl;
      return;
    }

    {
      auto lock = pause_mutex.async_lock_shared(yield[ec]);
      if (ec == boost::asio::error::operation_aborted) {
        return;
      } else if (ec) {
        ldout(cct, 1) << "failed to lock: " << ec.message() << dendl;
        return;
      }

      // process the request
      RGWRequest req{env.store->getRados()->get_new_req_id()};

      auto& socket = stream.lowest_layer();
      const auto& remote_endpoint = socket.remote_endpoint(ec);
      if (ec) {
        ldout(cct, 1) << "failed to connect client: " << ec.message() << dendl;
        return;
      }

      StreamIO real_client{cct, stream, parser, yield, buffer, is_ssl,
                           socket.local_endpoint(),
                           remote_endpoint};

      auto real_client_io = rgw::io::add_reordering(
                              rgw::io::add_buffering(cct,
                                rgw::io::add_chunking(
                                  rgw::io::add_conlen_controlling(
                                    &real_client))));
      RGWRestfulIO client(cct, &real_client_io);
      auto y = optional_yield{context, yield};
      process_request(env.store, env.rest, &req, env.uri_prefix,
                      *env.auth_registry, &client, env.olog, y, scheduler);
    }

    if (!parser.keep_alive()) {
      return;
    }

    // if we failed before reading the entire message, discard any remaining
    // bytes before reading the next
    while (!parser.is_done()) {
      static std::array<char, 1024> discard_buffer;

      auto& body = parser.get().body();
      body.size = discard_buffer.size();
      body.data = discard_buffer.data();

      http::async_read_some(stream, buffer, parser, yield[ec]);
      if (ec == http::error::need_buffer) {
        continue;
      }
      if (ec == boost::asio::error::connection_reset) {
        return;
      }
      if (ec) {
        ldout(cct, 5) << "failed to discard unread message: "
            << ec.message() << dendl;
        return;
      }
    }
  }
}
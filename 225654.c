int AsioFrontend::init()
{
  boost::system::error_code ec;
  auto& config = conf->get_config_map();

#ifdef WITH_RADOSGW_BEAST_OPENSSL
  int r = init_ssl();
  if (r < 0) {
    return r;
  }
#endif

  // parse endpoints
  auto ports = config.equal_range("port");
  for (auto i = ports.first; i != ports.second; ++i) {
    auto port = parse_port(i->second.c_str(), ec);
    if (ec) {
      lderr(ctx()) << "failed to parse port=" << i->second << dendl;
      return -ec.value();
    }
    listeners.emplace_back(context);
    listeners.back().endpoint.port(port);

    listeners.emplace_back(context);
    listeners.back().endpoint = tcp::endpoint(tcp::v6(), port);
  }

  auto endpoints = config.equal_range("endpoint");
  for (auto i = endpoints.first; i != endpoints.second; ++i) {
    auto endpoint = parse_endpoint(i->second, 80, ec);
    if (ec) {
      lderr(ctx()) << "failed to parse endpoint=" << i->second << dendl;
      return -ec.value();
    }
    listeners.emplace_back(context);
    listeners.back().endpoint = endpoint;
  }
  // parse tcp nodelay
  auto nodelay = config.find("tcp_nodelay");
  if (nodelay != config.end()) {
    for (auto& l : listeners) {
      l.use_nodelay = (nodelay->second == "1");
    }
  }
  

  bool socket_bound = false;
  // start listeners
  for (auto& l : listeners) {
    l.acceptor.open(l.endpoint.protocol(), ec);
    if (ec) {
      if (ec == boost::asio::error::address_family_not_supported) {
	ldout(ctx(), 0) << "WARNING: cannot open socket for endpoint=" << l.endpoint
			<< ", " << ec.message() << dendl;
	continue;
      }

      lderr(ctx()) << "failed to open socket: " << ec.message() << dendl;
      return -ec.value();
    }

    if (l.endpoint.protocol() == tcp::v6()) {
      l.acceptor.set_option(boost::asio::ip::v6_only(true), ec);
      if (ec) {
        lderr(ctx()) << "failed to set v6_only socket option: "
		     << ec.message() << dendl;
	return -ec.value();
      }
    }

    l.acceptor.set_option(tcp::acceptor::reuse_address(true));
    l.acceptor.bind(l.endpoint, ec);
    if (ec) {
      lderr(ctx()) << "failed to bind address " << l.endpoint
          << ": " << ec.message() << dendl;
      return -ec.value();
    }

    l.acceptor.listen(boost::asio::socket_base::max_connections);
    l.acceptor.async_accept(l.socket,
                            [this, &l] (boost::system::error_code ec) {
                              accept(l, ec);
                            });

    ldout(ctx(), 4) << "frontend listening on " << l.endpoint << dendl;
    socket_bound = true;
  }
  if (!socket_bound) {
    lderr(ctx()) << "Unable to listen at any endpoints" << dendl;
    return -EINVAL;
  }

  return drop_privileges(ctx());
}
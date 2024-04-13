int AsioFrontend::init_ssl()
{
  boost::system::error_code ec;
  auto& config = conf->get_config_map();

  // ssl configuration
  auto cert = config.find("ssl_certificate");
  const bool have_cert = cert != config.end();
  if (have_cert) {
    // only initialize the ssl context if it's going to be used
    ssl_context = boost::in_place(ssl::context::tls);
  }

  auto key = config.find("ssl_private_key");
  const bool have_private_key = key != config.end();
  if (have_private_key) {
    if (!have_cert) {
      lderr(ctx()) << "no ssl_certificate configured for ssl_private_key" << dendl;
      return -EINVAL;
    }
    ssl_context->use_private_key_file(key->second, ssl::context::pem, ec);
    if (ec) {
      lderr(ctx()) << "failed to add ssl_private_key=" << key->second
          << ": " << ec.message() << dendl;
      return -ec.value();
    }
  }
  if (have_cert) {
    ssl_context->use_certificate_chain_file(cert->second, ec);
    if (ec) {
      lderr(ctx()) << "failed to use ssl_certificate=" << cert->second
          << ": " << ec.message() << dendl;
      return -ec.value();
    }
    if (!have_private_key) {
      // attempt to use it as a private key if a separate one wasn't provided
      ssl_context->use_private_key_file(cert->second, ssl::context::pem, ec);
      if (ec) {
        lderr(ctx()) << "failed to use ssl_certificate=" << cert->second
            << " as a private key: " << ec.message() << dendl;
        return -ec.value();
      }
    }
  }

  // parse ssl endpoints
  auto ports = config.equal_range("ssl_port");
  for (auto i = ports.first; i != ports.second; ++i) {
    if (!have_cert) {
      lderr(ctx()) << "no ssl_certificate configured for ssl_port" << dendl;
      return -EINVAL;
    }
    auto port = parse_port(i->second.c_str(), ec);
    if (ec) {
      lderr(ctx()) << "failed to parse ssl_port=" << i->second << dendl;
      return -ec.value();
    }
    listeners.emplace_back(context);
    listeners.back().endpoint.port(port);
    listeners.back().use_ssl = true;

    listeners.emplace_back(context);
    listeners.back().endpoint = tcp::endpoint(tcp::v6(), port);
    listeners.back().use_ssl = true;
  }

  auto endpoints = config.equal_range("ssl_endpoint");
  for (auto i = endpoints.first; i != endpoints.second; ++i) {
    if (!have_cert) {
      lderr(ctx()) << "no ssl_certificate configured for ssl_endpoint" << dendl;
      return -EINVAL;
    }
    auto endpoint = parse_endpoint(i->second, 443, ec);
    if (ec) {
      lderr(ctx()) << "failed to parse ssl_endpoint=" << i->second << dendl;
      return -ec.value();
    }
    listeners.emplace_back(context);
    listeners.back().endpoint = endpoint;
    listeners.back().use_ssl = true;
  }
  return 0;
}
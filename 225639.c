tcp::endpoint parse_endpoint(boost::asio::string_view input,
                             unsigned short default_port,
                             boost::system::error_code& ec)
{
  tcp::endpoint endpoint;

  if (input.empty()) {
    ec = boost::asio::error::invalid_argument;
    return endpoint;
  }

  if (input[0] == '[') { // ipv6
    const size_t addr_begin = 1;
    const size_t addr_end = input.find(']');
    if (addr_end == input.npos) { // no matching ]
      ec = boost::asio::error::invalid_argument;
      return endpoint;
    }
    if (addr_end + 1 < input.size()) {
      // :port must must follow [ipv6]
      if (input[addr_end + 1] != ':') {
        ec = boost::asio::error::invalid_argument;
        return endpoint;
      } else {
        auto port_str = input.substr(addr_end + 2);
        endpoint.port(parse_port(port_str.data(), ec));
      }
    } else {
      endpoint.port(default_port);
    }
    auto addr = input.substr(addr_begin, addr_end - addr_begin);
    endpoint.address(boost::asio::ip::make_address_v6(addr, ec));
  } else { // ipv4
    auto colon = input.find(':');
    if (colon != input.npos) {
      auto port_str = input.substr(colon + 1);
      endpoint.port(parse_port(port_str.data(), ec));
      if (ec) {
        return endpoint;
      }
    } else {
      endpoint.port(default_port);
    }
    auto addr = input.substr(0, colon);
    endpoint.address(boost::asio::ip::make_address_v4(addr, ec));
  }
  return endpoint;
}
  void close(boost::system::error_code& ec) {
    std::lock_guard lock{mutex};
    for (auto& conn : connections) {
      conn.socket.close(ec);
    }
    connections.clear();
  }
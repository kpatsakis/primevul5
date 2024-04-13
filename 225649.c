  size_t write_data(const char* buf, size_t len) override {
    boost::system::error_code ec;
    auto bytes = boost::asio::async_write(stream, boost::asio::buffer(buf, len),
                                          yield[ec]);
    if (ec) {
      ldout(cct, 4) << "write_data failed: " << ec.message() << dendl;
      throw rgw::io::Exception(ec.value(), std::system_category());
    }
    return bytes;
  }
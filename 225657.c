    explicit Listener(boost::asio::io_context& context)
      : acceptor(context), socket(context) {}
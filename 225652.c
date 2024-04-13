void AsioFrontend::accept(Listener& l, boost::system::error_code ec)
{
  if (!l.acceptor.is_open()) {
    return;
  } else if (ec == boost::asio::error::operation_aborted) {
    return;
  } else if (ec) {
    ldout(ctx(), 1) << "accept failed: " << ec.message() << dendl;
    return;
  }
  auto socket = std::move(l.socket);
  tcp::no_delay options(l.use_nodelay);
  socket.set_option(options,ec);
  l.acceptor.async_accept(l.socket,
                          [this, &l] (boost::system::error_code ec) {
                            accept(l, ec);
                          });

  // spawn a coroutine to handle the connection
#ifdef WITH_RADOSGW_BEAST_OPENSSL
  if (l.use_ssl) {
    spawn::spawn(context,
      [this, s=std::move(socket)] (spawn::yield_context yield) mutable {
        Connection conn{s};
        auto c = connections.add(conn);
        // wrap the socket in an ssl stream
        ssl::stream<tcp::socket&> stream{s, *ssl_context};
        auto buffer = std::make_unique<parse_buffer>();
        // do ssl handshake
        boost::system::error_code ec;
        auto bytes = stream.async_handshake(ssl::stream_base::server,
                                            buffer->data(), yield[ec]);
        if (ec) {
          ldout(ctx(), 1) << "ssl handshake failed: " << ec.message() << dendl;
          return;
        }
        buffer->consume(bytes);
        handle_connection(context, env, stream, *buffer, true, pause_mutex,
                          scheduler.get(), ec, yield);
        if (!ec) {
          // ssl shutdown (ignoring errors)
          stream.async_shutdown(yield[ec]);
        }
        s.shutdown(tcp::socket::shutdown_both, ec);
      }, make_stack_allocator());
  } else {
#else
  {
#endif // WITH_RADOSGW_BEAST_OPENSSL
    spawn::spawn(context,
      [this, s=std::move(socket)] (spawn::yield_context yield) mutable {
        Connection conn{s};
        auto c = connections.add(conn);
        auto buffer = std::make_unique<parse_buffer>();
        boost::system::error_code ec;
        handle_connection(context, env, s, *buffer, false, pause_mutex,
                          scheduler.get(), ec, yield);
        s.shutdown(tcp::socket::shutdown_both, ec);
      }, make_stack_allocator());
  }
}

int AsioFrontend::run()
{
  auto cct = ctx();
  const int thread_count = cct->_conf->rgw_thread_pool_size;
  threads.reserve(thread_count);

  ldout(cct, 4) << "frontend spawning " << thread_count << " threads" << dendl;

  // the worker threads call io_context::run(), which will return when there's
  // no work left. hold a work guard to keep these threads going until join()
  work.emplace(boost::asio::make_work_guard(context));

  for (int i = 0; i < thread_count; i++) {
    threads.emplace_back([=] {
      // request warnings on synchronous librados calls in this thread
      is_asio_thread = true;
      boost::system::error_code ec;
      context.run(ec);
    });
  }
  return 0;
}

void AsioFrontend::stop()
{
  ldout(ctx(), 4) << "frontend initiating shutdown..." << dendl;

  going_down = true;

  boost::system::error_code ec;
  // close all listeners
  for (auto& listener : listeners) {
    listener.acceptor.close(ec);
  }
  // close all connections
  connections.close(ec);
  pause_mutex.cancel();
}

void AsioFrontend::join()
{
  if (!going_down) {
    stop();
  }
  work.reset();

  ldout(ctx(), 4) << "frontend joining threads..." << dendl;
  for (auto& thread : threads) {
    thread.join();
  }
  ldout(ctx(), 4) << "frontend done" << dendl;
}

void AsioFrontend::pause()
{
  ldout(ctx(), 4) << "frontend pausing connections..." << dendl;

  // cancel pending calls to accept(), but don't close the sockets
  boost::system::error_code ec;
  for (auto& l : listeners) {
    l.acceptor.cancel(ec);
  }

  // pause and wait for outstanding requests to complete
  pause_mutex.lock(ec);

  if (ec) {
    ldout(ctx(), 1) << "frontend failed to pause: " << ec.message() << dendl;
  } else {
    ldout(ctx(), 4) << "frontend paused" << dendl;
  }
}

void AsioFrontend::unpause(rgw::sal::RGWRadosStore* const store,
                           rgw_auth_registry_ptr_t auth_registry)
{
  env.store = store;
  env.auth_registry = std::move(auth_registry);

  // unpause to unblock connections
  pause_mutex.unlock();

  // start accepting connections again
  for (auto& l : listeners) {
    l.acceptor.async_accept(l.socket,
                            [this, &l] (boost::system::error_code ec) {
                              accept(l, ec);
                            });
  }

  ldout(ctx(), 4) << "frontend unpaused" << dendl;
}

} // anonymous namespace
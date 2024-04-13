void ProtocolV1::reset_recv_state()
{
  ldout(cct, 5) << __func__ << dendl;

  // execute in the same thread that uses the `session_security`.
  // We need to do the warp because holding `write_lock` is not
  // enough as `write_event()` releases it just before calling
  // `write_message()`. `submit_to()` here is NOT blocking.
  if (!connection->center->in_thread()) {
    connection->center->submit_to(connection->center->get_id(), [this] {
      ldout(cct, 5) << "reset_recv_state (warped) reseting security handlers"
                    << dendl;
      // Possibly unnecessary. See the comment in `deactivate_existing`.
      std::lock_guard<std::mutex> l(connection->lock);
      std::lock_guard<std::mutex> wl(connection->write_lock);
      reset_security();
    }, /* always_async = */true);
  } else {
    reset_security();
  }

  // clean read and write callbacks
  connection->pendingReadLen.reset();
  connection->writeCallback.reset();

  if (state > THROTTLE_MESSAGE && state <= READ_FOOTER_AND_DISPATCH &&
      connection->policy.throttler_messages) {
    ldout(cct, 10) << __func__ << " releasing " << 1
                   << " message to policy throttler "
                   << connection->policy.throttler_messages->get_current()
                   << "/" << connection->policy.throttler_messages->get_max()
                   << dendl;
    connection->policy.throttler_messages->put();
  }
  if (state > THROTTLE_BYTES && state <= READ_FOOTER_AND_DISPATCH) {
    if (connection->policy.throttler_bytes) {
      ldout(cct, 10) << __func__ << " releasing " << cur_msg_size
                     << " bytes to policy throttler "
                     << connection->policy.throttler_bytes->get_current() << "/"
                     << connection->policy.throttler_bytes->get_max() << dendl;
      connection->policy.throttler_bytes->put(cur_msg_size);
    }
  }
  if (state > THROTTLE_DISPATCH_QUEUE && state <= READ_FOOTER_AND_DISPATCH) {
    ldout(cct, 10)
        << __func__ << " releasing " << cur_msg_size
        << " bytes to dispatch_queue throttler "
        << connection->dispatch_queue->dispatch_throttler.get_current() << "/"
        << connection->dispatch_queue->dispatch_throttler.get_max() << dendl;
    connection->dispatch_queue->dispatch_throttle_release(cur_msg_size);
  }
}
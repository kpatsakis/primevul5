int RGWSwiftWebsiteHandler::retarget_object(RGWOp* op, RGWOp** new_op)
{
  ldout(s->cct, 10) << "Starting object retarget" << dendl;
  RGWOp* op_override = nullptr;

  /* In Swift static web content is served if the request is anonymous or
   * has X-Web-Mode HTTP header specified to true. */
  if (can_be_website_req() && is_web_dir()) {
    const auto& ws_conf = s->bucket_info.website_conf;
    const auto& index = s->bucket_info.website_conf.get_index_doc();

    if (s->decoded_uri.back() != '/') {
      op_override = get_ws_redirect_op();
    } else if (! index.empty() && is_index_present(index)) {
      op_override = get_ws_index_op();
    } else if (ws_conf.listing_enabled) {
      op_override = get_ws_listing_op();
    }
  } else {
    /* A regular request or the specified object isn't a subdirectory marker.
     * We don't need any re-targeting. Error handling (like sending a custom
     * error page) will be performed by error_handler of the actual RGWOp. */
    return 0;
  }

  if (op_override) {
    handler->put_op(op);
    op_override->init(store, s, handler);

    *new_op = op_override;
  } else {
    *new_op = op;
  }

  /* Return 404 Not Found if we aren't able to re-target for subdir marker. */
  return ! op_override ? -ENOENT : 0;
}
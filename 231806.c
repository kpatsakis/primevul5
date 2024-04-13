int RGWSwiftWebsiteHandler::error_handler(const int err_no,
                                          std::string* const error_content)
{
  const auto& ws_conf = s->bucket_info.website_conf;

  if (can_be_website_req() && ! ws_conf.error_doc.empty()) {
    set_req_state_err(s, err_no);
    return serve_errordoc(s->err.http_ret, ws_conf.error_doc);
  }

  /* Let's go to the default, no-op handler. */
  return err_no;
}
bool RGWSwiftWebsiteHandler::can_be_website_req() const
{
  /* Static website works only with the GET or HEAD method. Nothing more. */
  static const std::set<boost::string_ref> ws_methods = { "GET", "HEAD" };
  if (ws_methods.count(s->info.method) == 0) {
    return false;
  }

  /* We also need to handle early failures from the auth system. In such cases
   * req_state::auth.identity may be empty. Let's treat that the same way as
   * the anonymous access. */
  if (! s->auth.identity) {
    return true;
  }

  /* Swift serves websites only for anonymous requests unless client explicitly
   * requested this behaviour by supplying X-Web-Mode HTTP header set to true. */
  if (s->auth.identity->is_anonymous() || is_web_mode()) {
    return true;
  }

  return false;
}
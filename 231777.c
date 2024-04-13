bool RGWSwiftWebsiteHandler::is_web_mode() const
{
  const boost::string_ref webmode = s->info.env->get("HTTP_X_WEB_MODE", "");
  return boost::algorithm::iequals(webmode, "true");
}
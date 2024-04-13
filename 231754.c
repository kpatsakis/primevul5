bool RGWFormPost::is_formpost_req(req_state* const s)
{
  std::string content_type;
  std::map<std::string, std::string> params;

  parse_boundary_params(s->info.env->get("CONTENT_TYPE", ""),
                        content_type, params);

  return boost::algorithm::iequals(content_type, "multipart/form-data") &&
         params.count("boundary") > 0;
}
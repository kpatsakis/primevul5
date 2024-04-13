int RGWSetBucketWebsite_ObjStore_S3::get_params()
{
  const auto max_size = s->cct->_conf->rgw_max_put_param_size;

  int r = 0;
  bufferlist data;
  std::tie(r, data) = rgw_rest_read_all_input(s, max_size, false);

  if (r < 0) {
    return r;
  }

  r = do_aws4_auth_completion();
  if (r < 0) {
    return r;
  }

  in_data.append(data);

  RGWXMLDecoder::XMLParser parser;
  if (!parser.init()) {
    ldout(s->cct, 0) << "ERROR: failed to initialize parser" << dendl;
    return -EIO;
  }

  char* buf = data.c_str();
  if (!parser.parse(buf, data.length(), 1)) {
    ldout(s->cct, 5) << "failed to parse xml: " << buf << dendl;
    return -EINVAL;
  }

  try {
    RGWXMLDecoder::decode_xml("WebsiteConfiguration", website_conf, &parser, true);
  } catch (RGWXMLDecoder::err& err) {
    ldout(s->cct, 5) << "unexpected xml: " << buf << dendl;
    return -EINVAL;
  }

  if (website_conf.is_redirect_all && website_conf.redirect_all.hostname.empty()) {
    s->err.message = "A host name must be provided to redirect all requests (e.g. \"example.com\").";
    ldout(s->cct, 5) << s->err.message << dendl;
    return -EINVAL;
  } else if (!website_conf.is_redirect_all && !website_conf.is_set_index_doc) {
    s->err.message = "A value for IndexDocument Suffix must be provided if RedirectAllRequestsTo is empty";
    ldout(s->cct, 5) << s->err.message << dendl;
    return -EINVAL;
  } else if (!website_conf.is_redirect_all && website_conf.is_set_index_doc &&
             website_conf.index_doc_suffix.empty()) {
    s->err.message = "The IndexDocument Suffix is not well formed";
    ldout(s->cct, 5) << s->err.message << dendl;
    return -EINVAL;
  }

#define WEBSITE_ROUTING_RULES_MAX_NUM      50
  int max_num = s->cct->_conf->rgw_website_routing_rules_max_num;
  if (max_num < 0) {
    max_num = WEBSITE_ROUTING_RULES_MAX_NUM;
  }
  int routing_rules_num = website_conf.routing_rules.rules.size();
  if (routing_rules_num > max_num) {
    ldout(s->cct, 4) << "An website routing config can have up to "
                     << max_num
                     << " rules, request website routing rules num: "
                     << routing_rules_num << dendl;
    op_ret = -ERR_INVALID_WEBSITE_ROUTING_RULES_ERROR;
    s->err.message = std::to_string(routing_rules_num) +" routing rules provided, the number of routing rules in a website configuration is limited to "
                     + std::to_string(max_num)
                     + ".";
    return -ERR_INVALID_REQUEST;
  }

  return 0;
}
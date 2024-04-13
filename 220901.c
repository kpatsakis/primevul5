int RGWPutCORS_ObjStore_S3::get_params()
{
  RGWCORSXMLParser_S3 parser(s->cct);
  RGWCORSConfiguration_S3 *cors_config;

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

  if (!parser.init()) {
    return -EINVAL;
  }

  char* buf = data.c_str();
  if (!buf || !parser.parse(buf, data.length(), 1)) {
    return -ERR_MALFORMED_XML;
  }
  cors_config =
    static_cast<RGWCORSConfiguration_S3 *>(parser.find_first(
					     "CORSConfiguration"));
  if (!cors_config) {
    return -ERR_MALFORMED_XML;
  }

#define CORS_RULES_MAX_NUM      100
  int max_num = s->cct->_conf->rgw_cors_rules_max_num;
  if (max_num < 0) {
    max_num = CORS_RULES_MAX_NUM;
  }
  int cors_rules_num = cors_config->get_rules().size();
  if (cors_rules_num > max_num) {
    ldout(s->cct, 4) << "An cors config can have up to "
                     << max_num
                     << " rules, request cors rules num: "
                     << cors_rules_num << dendl;
    op_ret = -ERR_INVALID_CORS_RULES_ERROR;
    s->err.message = "The number of CORS rules should not exceed allowed limit of "
                     + std::to_string(max_num) + " rules.";
    return -ERR_INVALID_REQUEST;
  }

  // forward bucket cors requests to meta master zone
  if (!store->svc.zone->is_meta_master()) {
    /* only need to keep this data around if we're not meta master */
    in_data.append(data);
  }

  if (s->cct->_conf->subsys.should_gather<ceph_subsys_rgw, 15>()) {
    ldout(s->cct, 15) << "CORSConfiguration";
    cors_config->to_xml(*_dout);
    *_dout << dendl;
  }

  cors_config->encode(cors_bl);

  return 0;
}
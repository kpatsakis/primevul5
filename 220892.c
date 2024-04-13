RGWHandler_REST* RGWRESTMgr_S3::get_handler(struct req_state* const s,
                                            const rgw::auth::StrategyRegistry& auth_registry,
                                            const std::string& frontend_prefix)
{
  bool is_s3website = enable_s3website && (s->prot_flags & RGW_REST_WEBSITE);
  int ret =
    RGWHandler_REST_S3::init_from_header(s,
					is_s3website ? RGW_FORMAT_HTML :
					RGW_FORMAT_XML, true);
  if (ret < 0)
    return NULL;

  RGWHandler_REST* handler;
  // TODO: Make this more readable
  if (is_s3website) {
    if (s->init_state.url_bucket.empty()) {
      handler = new RGWHandler_REST_Service_S3Website(auth_registry);
    } else if (s->object.empty()) {
      handler = new RGWHandler_REST_Bucket_S3Website(auth_registry);
    } else {
      handler = new RGWHandler_REST_Obj_S3Website(auth_registry);
    }
  } else {
    if (s->init_state.url_bucket.empty()) {
      handler = new RGWHandler_REST_Service_S3(auth_registry, enable_sts, enable_iam, enable_pubsub);
    } else if (s->object.empty()) {
      handler = new RGWHandler_REST_Bucket_S3(auth_registry, enable_pubsub);
    } else {
      handler = new RGWHandler_REST_Obj_S3(auth_registry);
    }
  }

  ldout(s->cct, 20) << __func__ << " handler=" << typeid(*handler).name()
		    << dendl;
  return handler;
}
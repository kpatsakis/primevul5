RGWRESTMgr_SWIFT::get_handler(struct req_state* const s,
                              const rgw::auth::StrategyRegistry& auth_registry,
                              const std::string& frontend_prefix)
{
  int ret = RGWHandler_REST_SWIFT::init_from_header(s, frontend_prefix);
  if (ret < 0) {
    ldout(s->cct, 10) << "init_from_header returned err=" << ret <<  dendl;
    return nullptr;
  }

  const auto& auth_strategy = auth_registry.get_swift();

  if (s->init_state.url_bucket.empty()) {
    return new RGWHandler_REST_Service_SWIFT(auth_strategy);
  }

  if (s->object.empty()) {
    return new RGWHandler_REST_Bucket_SWIFT(auth_strategy);
  }

  return new RGWHandler_REST_Obj_SWIFT(auth_strategy);
}
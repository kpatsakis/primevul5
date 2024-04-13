RGWHandler_REST* RGWRESTMgr_SWIFT_Info::get_handler(
  struct req_state* const s,
  const rgw::auth::StrategyRegistry& auth_registry,
  const std::string& frontend_prefix)
{
  s->prot_flags |= RGW_REST_SWIFT;
  const auto& auth_strategy = auth_registry.get_swift();
  return new RGWHandler_REST_SWIFT_Info(auth_strategy);
}
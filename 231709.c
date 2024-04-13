int RGWHandler_REST_SWIFT::authorize(const DoutPrefixProvider *dpp)
{
  return rgw::auth::Strategy::apply(dpp, auth_strategy, s);
}
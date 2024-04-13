int RGW_Auth_S3::authorize(const DoutPrefixProvider *dpp,
                           RGWRados* const store,
                           const rgw::auth::StrategyRegistry& auth_registry,
                           struct req_state* const s)
{

  /* neither keystone and rados enabled; warn and exit! */
  if (!store->ctx()->_conf->rgw_s3_auth_use_rados &&
      !store->ctx()->_conf->rgw_s3_auth_use_keystone &&
      !store->ctx()->_conf->rgw_s3_auth_use_ldap) {
    ldpp_dout(dpp, 0) << "WARNING: no authorization backend enabled! Users will never authenticate." << dendl;
    return -EPERM;
  }

  const auto ret = rgw::auth::Strategy::apply(dpp, auth_registry.get_s3_main(), s);
  if (ret == 0) {
    /* Populate the owner info. */
    s->owner.set_id(s->user->user_id);
    s->owner.set_name(s->user->display_name);
  }
  return ret;
}
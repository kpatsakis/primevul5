void RGWFormPost::get_owner_info(const req_state* const s,
                                   RGWUserInfo& owner_info) const
{
  /* We cannot use req_state::bucket_name because it isn't available
   * now. It will be initialized in RGWHandler_REST_SWIFT::postauth_init(). */
  const string& bucket_name = s->init_state.url_bucket;

  /* TempURL in Formpost only requires that bucket name is specified. */
  if (bucket_name.empty()) {
    throw -EPERM;
  }

  string bucket_tenant;
  if (!s->account_name.empty()) {
    RGWUserInfo uinfo;
    bool found = false;

    const rgw_user uid(s->account_name);
    if (uid.tenant.empty()) {
      const rgw_user tenanted_uid(uid.id, uid.id);

      if (rgw_get_user_info_by_uid(store, tenanted_uid, uinfo) >= 0) {
        /* Succeeded. */
        bucket_tenant = uinfo.user_id.tenant;
        found = true;
      }
    }

    if (!found && rgw_get_user_info_by_uid(store, uid, uinfo) < 0) {
      throw -EPERM;
    } else {
      bucket_tenant = uinfo.user_id.tenant;
    }
  }

  /* Need to get user info of bucket owner. */
  RGWBucketInfo bucket_info;
  int ret = store->get_bucket_info(*s->sysobj_ctx,
                                   bucket_tenant, bucket_name,
                                   bucket_info, nullptr);
  if (ret < 0) {
    throw ret;
  }

  ldout(s->cct, 20) << "temp url user (bucket owner): " << bucket_info.owner
                 << dendl;

  if (rgw_get_user_info_by_uid(store, bucket_info.owner, owner_info) < 0) {
    throw -EPERM;
  }
}
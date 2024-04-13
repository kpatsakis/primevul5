int RGWHandler_REST_SWIFT::postauth_init()
{
  struct req_init_state* t = &s->init_state;

  /* XXX Stub this until Swift Auth sets account into URL. */
  s->bucket_tenant = s->user->user_id.tenant;
  s->bucket_name = t->url_bucket;

  dout(10) << "s->object=" <<
    (!s->object.empty() ? s->object : rgw_obj_key("<NULL>"))
           << " s->bucket="
	   << rgw_make_bucket_entry_name(s->bucket_tenant, s->bucket_name)
	   << dendl;

  int ret;
  ret = rgw_validate_tenant_name(s->bucket_tenant);
  if (ret)
    return ret;
  ret = validate_bucket_name(s->bucket_name);
  if (ret)
    return ret;
  ret = validate_object_name(s->object.name);
  if (ret)
    return ret;

  if (!t->src_bucket.empty()) {
    /*
     * We don't allow cross-tenant copy at present. It requires account
     * names in the URL for Swift.
     */
    s->src_tenant_name = s->user->user_id.tenant;
    s->src_bucket_name = t->src_bucket;

    ret = validate_bucket_name(s->src_bucket_name);
    if (ret < 0) {
      return ret;
    }
    ret = validate_object_name(s->src_object.name);
    if (ret < 0) {
      return ret;
    }
  }

  return 0;
}
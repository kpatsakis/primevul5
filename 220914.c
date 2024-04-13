int RGWHandler_REST_S3::init(RGWRados *store, struct req_state *s,
                             rgw::io::BasicClient *cio)
{
  int ret;

  s->dialect = "s3";

  ret = rgw_validate_tenant_name(s->bucket_tenant);
  if (ret)
    return ret;
  bool relaxed_names = s->cct->_conf->rgw_relaxed_s3_bucket_names;
  if (!s->bucket_name.empty()) {
    ret = valid_s3_bucket_name(s->bucket_name, relaxed_names);
    if (ret)
      return ret;
    ret = validate_object_name(s->object.name);
    if (ret)
      return ret;
  }

  const char *cacl = s->info.env->get("HTTP_X_AMZ_ACL");
  if (cacl)
    s->canned_acl = cacl;

  s->has_acl_header = s->info.env->exists_prefix("HTTP_X_AMZ_GRANT");

  const char *copy_source = s->info.env->get("HTTP_X_AMZ_COPY_SOURCE");
  if (copy_source &&
      (! s->info.env->get("HTTP_X_AMZ_COPY_SOURCE_RANGE")) &&
      (! s->info.args.exists("uploadId"))) {

    ret = RGWCopyObj::parse_copy_location(copy_source,
                                          s->init_state.src_bucket,
                                          s->src_object);
    if (!ret) {
      ldout(s->cct, 0) << "failed to parse copy location" << dendl;
      return -EINVAL; // XXX why not -ERR_INVALID_BUCKET_NAME or -ERR_BAD_URL?
    }
  }

  const char *sc = s->info.env->get("HTTP_X_AMZ_STORAGE_CLASS");
  if (sc) {
    s->info.storage_class = sc;
  }

  return RGWHandler_REST::init(store, s, cio);
}
int RGWCopyObj_ObjStore_SWIFT::get_params()
{
  if_mod = s->info.env->get("HTTP_IF_MODIFIED_SINCE");
  if_unmod = s->info.env->get("HTTP_IF_UNMODIFIED_SINCE");
  if_match = s->info.env->get("HTTP_COPY_IF_MATCH");
  if_nomatch = s->info.env->get("HTTP_COPY_IF_NONE_MATCH");

  src_tenant_name = s->src_tenant_name;
  src_bucket_name = s->src_bucket_name;
  src_object = s->src_object;
  dest_tenant_name = s->bucket_tenant;
  dest_bucket_name = s->bucket_name;
  dest_object = s->object.name;

  const char * const fresh_meta = s->info.env->get("HTTP_X_FRESH_METADATA");
  if (fresh_meta && strcasecmp(fresh_meta, "TRUE") == 0) {
    attrs_mod = RGWRados::ATTRSMOD_REPLACE;
  } else {
    attrs_mod = RGWRados::ATTRSMOD_MERGE;
  }

  int r = get_delete_at_param(s, delete_at);
  if (r < 0) {
    ldout(s->cct, 5) << "ERROR: failed to get Delete-At param" << dendl;
    return r;
  }

  return 0;
}
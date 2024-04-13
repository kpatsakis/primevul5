int RGWPutMetadataObject_ObjStore_SWIFT::get_params()
{
  if (s->has_bad_meta) {
    return -EINVAL;
  }

  /* Handle Swift object expiration. */
  int r = get_delete_at_param(s, delete_at);
  if (r < 0) {
    ldout(s->cct, 5) << "ERROR: failed to get Delete-At param" << dendl;
    return r;
  }

  dlo_manifest = s->info.env->get("HTTP_X_OBJECT_MANIFEST");

  return 0;
}
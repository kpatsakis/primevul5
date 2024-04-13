int RGWPutMetadataBucket_ObjStore_SWIFT::get_params()
{
  if (s->has_bad_meta) {
    return -EINVAL;
  }

  int r = get_swift_container_settings(s, store, &policy, &has_policy,
				       &policy_rw_mask, &cors_config, &has_cors);
  if (r < 0) {
    return r;
  }

  get_rmattrs_from_headers(s, CONT_PUT_ATTR_PREFIX, CONT_REMOVE_ATTR_PREFIX,
			   rmattr_names);
  placement_rule.init(s->info.env->get("HTTP_X_STORAGE_POLICY", ""), s->info.storage_class);

  return get_swift_versioning_settings(s, swift_ver_location);
}
int RGWCreateBucket_ObjStore_SWIFT::get_params()
{
  bool has_policy;
  uint32_t policy_rw_mask = 0;

  int r = get_swift_container_settings(s, store, &policy, &has_policy,
				       &policy_rw_mask, &cors_config, &has_cors);
  if (r < 0) {
    return r;
  }

  if (!has_policy) {
    policy.create_default(s->user->user_id, s->user->display_name);
  }

  location_constraint = store->svc.zone->get_zonegroup().api_name;
  get_rmattrs_from_headers(s, CONT_PUT_ATTR_PREFIX,
                           CONT_REMOVE_ATTR_PREFIX, rmattr_names);
  placement_rule.init(s->info.env->get("HTTP_X_STORAGE_POLICY", ""), s->info.storage_class);

  return get_swift_versioning_settings(s, swift_ver_location);
}
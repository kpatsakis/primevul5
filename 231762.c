void RGWInfo_ObjStore_SWIFT::list_swift_data(Formatter& formatter,
                                              const ConfigProxy& config,
                                              RGWRados& store)
{
  formatter.open_object_section("swift");
  formatter.dump_int("max_file_size", config->rgw_max_put_size);
  formatter.dump_int("container_listing_limit", RGW_LIST_BUCKETS_LIMIT_MAX);

  string ceph_version(CEPH_GIT_NICE_VER);
  formatter.dump_string("version", ceph_version);

  const size_t max_attr_name_len = \
    g_conf().get_val<Option::size_t>("rgw_max_attr_name_len");
  if (max_attr_name_len) {
    const size_t meta_name_limit = \
      max_attr_name_len - strlen(RGW_ATTR_PREFIX RGW_AMZ_META_PREFIX);
    formatter.dump_int("max_meta_name_length", meta_name_limit);
  }

  const size_t meta_value_limit = g_conf().get_val<Option::size_t>("rgw_max_attr_size");
  if (meta_value_limit) {
    formatter.dump_int("max_meta_value_length", meta_value_limit);
  }

  const size_t meta_num_limit = \
    g_conf().get_val<uint64_t>("rgw_max_attrs_num_in_req");
  if (meta_num_limit) {
    formatter.dump_int("max_meta_count", meta_num_limit);
  }

  formatter.open_array_section("policies");
  const RGWZoneGroup& zonegroup = store.svc.zone->get_zonegroup();

  for (const auto& placement_targets : zonegroup.placement_targets) {
    formatter.open_object_section("policy");
    if (placement_targets.second.name.compare(zonegroup.default_placement.name) == 0)
      formatter.dump_bool("default", true);
    formatter.dump_string("name", placement_targets.second.name.c_str());
    formatter.close_section();
  }
  formatter.close_section();

  formatter.dump_int("max_object_name_size", RGWHandler_REST::MAX_OBJ_NAME_LEN);
  formatter.dump_bool("strict_cors_mode", true);
  formatter.dump_int("max_container_name_length", RGWHandler_REST::MAX_BUCKET_NAME_LEN);
  formatter.close_section();
}
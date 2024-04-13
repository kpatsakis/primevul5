void RGWInfo_ObjStore_SWIFT::list_slo_data(Formatter& formatter,
                                            const ConfigProxy& config,
                                            RGWRados& store)
{
  formatter.open_object_section("slo");
  formatter.dump_int("max_manifest_segments", config->rgw_max_slo_entries);
  formatter.close_section();
}
void RGWInfo_ObjStore_SWIFT::list_tempauth_data(Formatter& formatter,
                                                 const ConfigProxy& config,
                                                 RGWRados& store)
{
  formatter.open_object_section("tempauth");
  formatter.dump_bool("account_acls", true);
  formatter.close_section();
}
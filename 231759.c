void RGWInfo_ObjStore_SWIFT::list_tempurl_data(Formatter& formatter,
                                                const ConfigProxy& config,
                                                RGWRados& store)
{
  formatter.open_object_section("tempurl");
  formatter.open_array_section("methods");
  formatter.dump_string("methodname", "GET");
  formatter.dump_string("methodname", "HEAD");
  formatter.dump_string("methodname", "PUT");
  formatter.dump_string("methodname", "POST");
  formatter.dump_string("methodname", "DELETE");
  formatter.close_section();
  formatter.close_section();
}
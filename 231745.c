int RGWDeleteObj_ObjStore_SWIFT::get_params()
{
  const string& mm = s->info.args.get("multipart-manifest");
  multipart_delete = (mm.compare("delete") == 0);

  return RGWDeleteObj_ObjStore::get_params();
}
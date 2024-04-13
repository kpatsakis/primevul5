int RGWGetObj_ObjStore_SWIFT::get_params()
{
  const string& mm = s->info.args.get("multipart-manifest");
  skip_manifest = (mm.compare("get") == 0);

  return RGWGetObj_ObjStore::get_params();
}
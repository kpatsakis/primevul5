void RGWInfo_ObjStore_SWIFT::execute()
{
  bool is_admin_info_enabled = false;

  const string& swiftinfo_sig = s->info.args.get("swiftinfo_sig");
  const string& swiftinfo_expires = s->info.args.get("swiftinfo_expires");

  if (!swiftinfo_sig.empty() &&
      !swiftinfo_expires.empty() &&
      !is_expired(swiftinfo_expires, s->cct)) {
    is_admin_info_enabled = true;
  }

  s->formatter->open_object_section("info");

  for (const auto& pair : swift_info) {
    if(!is_admin_info_enabled && pair.second.is_admin_info)
      continue;

    if (!pair.second.list_data) {
      s->formatter->open_object_section((pair.first).c_str());
      s->formatter->close_section();
    }
    else {
      pair.second.list_data(*(s->formatter), s->cct->_conf, *store);
    }
  }

  s->formatter->close_section();
}
bool RGWFormPost::is_integral()
{
  const std::string form_signature = get_part_str(ctrl_parts, "signature");

  try {
    get_owner_info(s, *s->user);
    s->auth.identity = rgw::auth::transform_old_authinfo(s);
  } catch (...) {
    ldout(s->cct, 5) << "cannot get user_info of account's owner" << dendl;
    return false;
  }

  for (const auto& kv : s->user->temp_url_keys) {
    const int temp_url_key_num = kv.first;
    const string& temp_url_key = kv.second;

    if (temp_url_key.empty()) {
      continue;
    }

    SignatureHelper sig_helper;
    sig_helper.calc(temp_url_key,
                    s->info.request_uri,
                    get_part_str(ctrl_parts, "redirect"),
                    get_part_str(ctrl_parts, "max_file_size", "0"),
                    get_part_str(ctrl_parts, "max_file_count", "0"),
                    get_part_str(ctrl_parts, "expires", "0"));

    const auto local_sig = sig_helper.get_signature();

    ldout(s->cct, 20) << "FormPost signature [" << temp_url_key_num << "]"
                      << " (calculated): " << local_sig << dendl;

    if (sig_helper.is_equal_to(form_signature)) {
      return true;
    } else {
      ldout(s->cct, 5) << "FormPost's signature mismatch: "
                       << local_sig << " != " << form_signature << dendl;
    }
  }

  return false;
}
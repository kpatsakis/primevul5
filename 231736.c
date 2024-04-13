static void get_rmattrs_from_headers(const req_state * const s,
				     const char * const put_prefix,
				     const char * const del_prefix,
				     set<string>& rmattr_names)
{
  const size_t put_prefix_len = strlen(put_prefix);
  const size_t del_prefix_len = strlen(del_prefix);

  for (const auto& kv : s->info.env->get_map()) {
    size_t prefix_len = 0;
    const char * const p = kv.first.c_str();

    if (strncasecmp(p, del_prefix, del_prefix_len) == 0) {
      /* Explicitly requested removal. */
      prefix_len = del_prefix_len;
    } else if ((strncasecmp(p, put_prefix, put_prefix_len) == 0)
	       && kv.second.empty()) {
      /* Removal requested by putting an empty value. */
      prefix_len = put_prefix_len;
    }

    if (prefix_len > 0) {
      string name(RGW_ATTR_META_PREFIX);
      name.append(lowercase_dash_http_attr(p + prefix_len));
      rmattr_names.insert(name);
    }
  }
}
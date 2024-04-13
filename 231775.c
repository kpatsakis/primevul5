static void dump_object_metadata(struct req_state * const s,
				 const map<string, bufferlist>& attrs)
{
  map<string, string> response_attrs;

  for (auto kv : attrs) {
    const char * name = kv.first.c_str();
    const auto aiter = rgw_to_http_attrs.find(name);

    if (aiter != std::end(rgw_to_http_attrs)) {
      response_attrs[aiter->second] = rgw_bl_str(kv.second);
    } else if (strcmp(name, RGW_ATTR_SLO_UINDICATOR) == 0) {
      // this attr has an extra length prefix from encode() in prior versions
      dump_header(s, "X-Object-Meta-Static-Large-Object", "True");
    } else if (strncmp(name, RGW_ATTR_META_PREFIX,
		       sizeof(RGW_ATTR_META_PREFIX)-1) == 0) {
      name += sizeof(RGW_ATTR_META_PREFIX) - 1;
      dump_header_prefixed(s, "X-Object-Meta-",
                           camelcase_dash_http_attr(name), kv.second);
    }
  }

  /* Handle override and fallback for Content-Disposition HTTP header.
   * At the moment this will be used only by TempURL of the Swift API. */
  const auto cditer = rgw_to_http_attrs.find(RGW_ATTR_CONTENT_DISP);
  if (cditer != std::end(rgw_to_http_attrs)) {
    const auto& name = cditer->second;

    if (!s->content_disp.override.empty()) {
      response_attrs[name] = s->content_disp.override;
    } else if (!s->content_disp.fallback.empty()
        && response_attrs.find(name) == std::end(response_attrs)) {
      response_attrs[name] = s->content_disp.fallback;
    }
  }

  for (const auto kv : response_attrs) {
    dump_header(s, kv.first, kv.second);
  }

  const auto iter = attrs.find(RGW_ATTR_DELETE_AT);
  if (iter != std::end(attrs)) {
    utime_t delete_at;
    try {
      decode(delete_at, iter->second);
      if (!delete_at.is_zero()) {
        dump_header(s, "X-Delete-At", delete_at.sec());
      }
    } catch (buffer::error& err) {
      ldout(s->cct, 0) << "ERROR: cannot decode object's " RGW_ATTR_DELETE_AT
                          " attr, ignoring"
                       << dendl;
    }
  }
}
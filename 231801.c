int RGWGetObj_ObjStore_SWIFT::send_response_data(bufferlist& bl,
                                                 const off_t bl_ofs,
                                                 const off_t bl_len)
{
  string content_type;

  if (sent_header) {
    goto send_data;
  }

  if (custom_http_ret) {
    set_req_state_err(s, 0);
    dump_errno(s, custom_http_ret);
  } else {
    set_req_state_err(s, (partial_content && !op_ret) ? STATUS_PARTIAL_CONTENT
		    : op_ret);
    dump_errno(s);

    if (s->is_err()) {
      end_header(s, NULL);
      return 0;
    }
  }

  if (range_str) {
    dump_range(s, ofs, end, s->obj_size);
  }

  if (s->is_err()) {
    end_header(s, NULL);
    return 0;
  }

  dump_content_length(s, total_len);
  dump_last_modified(s, lastmod);
  dump_header(s, "X-Timestamp", utime_t(lastmod));
  if (is_slo) {
    dump_header(s, "X-Static-Large-Object", "True");
  }

  if (! op_ret) {
    if (! lo_etag.empty()) {
      dump_etag(s, lo_etag, true /* quoted */);
    } else {
      auto iter = attrs.find(RGW_ATTR_ETAG);
      if (iter != attrs.end()) {
        dump_etag(s, iter->second.to_str());
      }
    }

    get_contype_from_attrs(attrs, content_type);
    dump_object_metadata(s, attrs);
  }

  end_header(s, this, !content_type.empty() ? content_type.c_str()
	     : "binary/octet-stream");

  sent_header = true;

send_data:
  if (get_data && !op_ret) {
    const auto r = dump_body(s, bl.c_str() + bl_ofs, bl_len);
    if (r < 0) {
      return r;
    }
  }
  rgw_flush_formatter_and_reset(s, s->formatter);

  return 0;
}
void RGWCopyObj_ObjStore_SWIFT::send_response()
{
  if (! sent_header) {
    string content_type;
    if (! op_ret)
      op_ret = STATUS_CREATED;
    set_req_state_err(s, op_ret);
    dump_errno(s);
    dump_etag(s, etag);
    dump_last_modified(s, mtime);
    dump_copy_info();
    get_contype_from_attrs(attrs, content_type);
    dump_object_metadata(s, attrs);
    end_header(s, this, !content_type.empty() ? content_type.c_str()
	       : "binary/octet-stream");
  } else {
    s->formatter->close_section();
    rgw_flush_formatter(s, s->formatter);
  }
}
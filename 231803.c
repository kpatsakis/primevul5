void RGWPutObj_ObjStore_SWIFT::send_response()
{
  const auto meta_ret = handle_metadata_errors(s, op_ret);
  if (meta_ret) {
    op_ret = meta_ret;
  } else {
    if (!op_ret) {
      op_ret = STATUS_CREATED;
    }
    set_req_state_err(s, op_ret);
  }

  if (! lo_etag.empty()) {
    /* Static Large Object of Swift API has two etags represented by
     * following members:
     *  - etag - for the manifest itself (it will be stored in xattrs),
     *  - lo_etag - for the content composited from SLO's segments.
     *    The value is calculated basing on segments' etags.
     * In response for PUT request we have to expose the second one.
     * The first one may be obtained by GET with "multipart-manifest=get"
     * in query string on a given SLO. */
    dump_etag(s, lo_etag, true /* quoted */);
  } else {
    dump_etag(s, etag);
  }

  dump_last_modified(s, mtime);
  set_req_state_err(s, op_ret);
  dump_errno(s);
  end_header(s, this);
  rgw_flush_formatter_and_reset(s, s->formatter);
}
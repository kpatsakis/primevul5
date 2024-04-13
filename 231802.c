void RGWCreateBucket_ObjStore_SWIFT::send_response()
{
  const auto meta_ret = handle_metadata_errors(s, op_ret);
  if (meta_ret != op_ret) {
    op_ret = meta_ret;
  } else {
    if (!op_ret) {
      op_ret = STATUS_CREATED;
    } else if (op_ret == -ERR_BUCKET_EXISTS) {
      op_ret = STATUS_ACCEPTED;
    }
    set_req_state_err(s, op_ret);
  }

  dump_errno(s);
  /* Propose ending HTTP header with 0 Content-Length header. */
  end_header(s, NULL, NULL, 0);
  rgw_flush_formatter_and_reset(s, s->formatter);
}
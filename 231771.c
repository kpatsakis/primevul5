void RGWPutMetadataAccount_ObjStore_SWIFT::send_response()
{
  const auto meta_ret = handle_metadata_errors(s, op_ret);
  if (meta_ret != op_ret) {
    op_ret = meta_ret;
  } else {
    if (!op_ret) {
      op_ret = STATUS_NO_CONTENT;
    }
    set_req_state_err(s, op_ret);
  }

  dump_errno(s);
  end_header(s, this);
  rgw_flush_formatter_and_reset(s, s->formatter);
}
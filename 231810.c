void RGWDeleteBucket_ObjStore_SWIFT::send_response()
{
  int r = op_ret;
  if (!r)
    r = STATUS_NO_CONTENT;

  set_req_state_err(s, r);
  dump_errno(s);
  end_header(s, this, NULL, 0);
  rgw_flush_formatter_and_reset(s, s->formatter);
}
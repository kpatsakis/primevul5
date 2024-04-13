void RGWStatBucket_ObjStore_SWIFT::send_response()
{
  if (op_ret >= 0) {
    op_ret = STATUS_NO_CONTENT;
    dump_container_metadata(s, bucket, bucket_quota,
                            s->bucket_info.website_conf);
  }

  set_req_state_err(s, op_ret);
  dump_errno(s);

  end_header(s, this, NULL, 0, true);
  dump_start(s);
}
void RGWPutObj_ObjStore_S3::send_response()
{
  if (op_ret) {
    set_req_state_err(s, op_ret);
    dump_errno(s);
  } else {
    if (s->cct->_conf->rgw_s3_success_create_obj_status) {
      op_ret = get_success_retcode(
	s->cct->_conf->rgw_s3_success_create_obj_status);
      set_req_state_err(s, op_ret);
    }
    if (copy_source.empty()) {
      dump_errno(s);
      dump_etag(s, etag);
      dump_content_length(s, 0);
      dump_header_if_nonempty(s, "x-amz-version-id", version_id);
      for (auto &it : crypt_http_responses)
        dump_header(s, it.first, it.second);
    } else {
      dump_errno(s);
      dump_header_if_nonempty(s, "x-amz-version-id", version_id);
      end_header(s, this, "application/xml");
      dump_start(s);
      struct tm tmp;
      utime_t ut(mtime);
      time_t secs = (time_t)ut.sec();
      gmtime_r(&secs, &tmp);
      char buf[TIME_BUF_SIZE];
      s->formatter->open_object_section_in_ns("CopyPartResult",
          "http://s3.amazonaws.com/doc/2006-03-01/");
      if (strftime(buf, sizeof(buf), "%Y-%m-%dT%T.000Z", &tmp) > 0) {
        s->formatter->dump_string("LastModified", buf);
      }
      s->formatter->dump_string("ETag", etag);
      s->formatter->close_section();
      rgw_flush_formatter_and_reset(s, s->formatter);
      return;
    }
  }
  if (append) {
    if (op_ret == 0 || op_ret == -ERR_POSITION_NOT_EQUAL_TO_LENGTH) {
      dump_header(s, "x-rgw-next-append-position", cur_accounted_size);
    }
  }
  if (s->system_request && !real_clock::is_zero(mtime)) {
    dump_epoch_header(s, "Rgwx-Mtime", mtime);
  }
  end_header(s, this);
}
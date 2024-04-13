void RGWListBuckets_ObjStore_SWIFT::send_response_begin(bool has_buckets)
{
  if (op_ret) {
    set_req_state_err(s, op_ret);
  } else if (!has_buckets && s->format == RGW_FORMAT_PLAIN) {
    op_ret = STATUS_NO_CONTENT;
    set_req_state_err(s, op_ret);
  }

  if (! s->cct->_conf->rgw_swift_enforce_content_length) {
    /* Adding account stats in the header to keep align with Swift API */
    dump_account_metadata(s,
            global_stats,
            policies_stats,
            attrs,
            user_quota,
            static_cast<RGWAccessControlPolicy_SWIFTAcct&>(*s->user_acl));
    dump_errno(s);
    dump_header(s, "Accept-Ranges", "bytes");
    end_header(s, NULL, NULL, NO_CONTENT_LENGTH, true);
  }

  if (! op_ret) {
    dump_start(s);
    s->formatter->open_array_section_with_attrs("account",
            FormatterAttrs("name", s->user->display_name.c_str(), NULL));

    sent_data = true;
  }
}
void RGWListBuckets_ObjStore_SWIFT::send_response_end()
{
  if (wants_reversed) {
    for (auto& buckets : reverse_buffer) {
      send_response_data_reversed(buckets);
    }
  }

  if (sent_data) {
    s->formatter->close_section();
  }

  if (s->cct->_conf->rgw_swift_enforce_content_length) {
    /* Adding account stats in the header to keep align with Swift API */
    dump_account_metadata(s,
            global_stats,
            policies_stats,
            attrs,
            user_quota,
            static_cast<RGWAccessControlPolicy_SWIFTAcct&>(*s->user_acl));
    dump_errno(s);
    end_header(s, nullptr, nullptr, s->formatter->get_len(), true);
  }

  if (sent_data || s->cct->_conf->rgw_swift_enforce_content_length) {
    rgw_flush_formatter_and_reset(s, s->formatter);
  }
}
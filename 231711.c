static void dump_account_metadata(struct req_state * const s,
                                  const RGWUsageStats& global_stats,
                                  const std::map<std::string, RGWUsageStats> &policies_stats,
                                  /* const */map<string, bufferlist>& attrs,
                                  const RGWQuotaInfo& quota,
                                  const RGWAccessControlPolicy_SWIFTAcct &policy)
{
  /* Adding X-Timestamp to keep align with Swift API */
  dump_header(s, "X-Timestamp", ceph_clock_now());

  dump_header(s, "X-Account-Container-Count", global_stats.buckets_count);
  dump_header(s, "X-Account-Object-Count", global_stats.objects_count);
  dump_header(s, "X-Account-Bytes-Used", global_stats.bytes_used);
  dump_header(s, "X-Account-Bytes-Used-Actual", global_stats.bytes_used_rounded);

  for (const auto& kv : policies_stats) {
    const auto& policy_name = camelcase_dash_http_attr(kv.first);
    const auto& policy_stats = kv.second;

    dump_header_infixed(s, "X-Account-Storage-Policy-", policy_name,
                        "-Container-Count", policy_stats.buckets_count);
    dump_header_infixed(s, "X-Account-Storage-Policy-", policy_name,
                        "-Object-Count", policy_stats.objects_count);
    dump_header_infixed(s, "X-Account-Storage-Policy-", policy_name,
                        "-Bytes-Used", policy_stats.bytes_used);
    dump_header_infixed(s, "X-Account-Storage-Policy-", policy_name,
                        "-Bytes-Used-Actual", policy_stats.bytes_used_rounded);
  }

  /* Dump TempURL-related stuff */
  if (s->perm_mask == RGW_PERM_FULL_CONTROL) {
    auto iter = s->user->temp_url_keys.find(0);
    if (iter != std::end(s->user->temp_url_keys) && ! iter->second.empty()) {
      dump_header(s, "X-Account-Meta-Temp-Url-Key", iter->second);
    }

    iter = s->user->temp_url_keys.find(1);
    if (iter != std::end(s->user->temp_url_keys) && ! iter->second.empty()) {
      dump_header(s, "X-Account-Meta-Temp-Url-Key-2", iter->second);
    }
  }

  /* Dump quota headers. */
  if (quota.enabled) {
    if (quota.max_size >= 0) {
      dump_header(s, "X-Account-Meta-Quota-Bytes", quota.max_size);
    }

    /* Limit on the number of objects in a given account is a RadosGW's
     * extension. Swift's account quota WSGI filter doesn't support it. */
    if (quota.max_objects >= 0) {
      dump_header(s, "X-Account-Meta-Quota-Count", quota.max_objects);
    }
  }

  /* Dump user-defined metadata items and generic attrs. */
  const size_t PREFIX_LEN = sizeof(RGW_ATTR_META_PREFIX) - 1;
  map<string, bufferlist>::iterator iter;
  for (iter = attrs.lower_bound(RGW_ATTR_PREFIX); iter != attrs.end(); ++iter) {
    const char *name = iter->first.c_str();
    map<string, string>::const_iterator geniter = rgw_to_http_attrs.find(name);

    if (geniter != rgw_to_http_attrs.end()) {
      dump_header(s, geniter->second, iter->second);
    } else if (strncmp(name, RGW_ATTR_META_PREFIX, PREFIX_LEN) == 0) {
      dump_header_prefixed(s, "X-Account-Meta-",
                           camelcase_dash_http_attr(name + PREFIX_LEN),
                           iter->second);
    }
  }

  /* Dump account ACLs */
  auto account_acls = policy.to_str();
  if (account_acls) {
    dump_header(s, "X-Account-Access-Control", std::move(*account_acls));
  }
}
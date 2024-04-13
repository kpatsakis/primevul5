static void dump_container_metadata(struct req_state *s,
                                    const RGWBucketEnt& bucket,
                                    const RGWQuotaInfo& quota,
                                    const RGWBucketWebsiteConf& ws_conf)
{
  /* Adding X-Timestamp to keep align with Swift API */
  dump_header(s, "X-Timestamp", utime_t(s->bucket_info.creation_time));

  dump_header(s, "X-Container-Object-Count", bucket.count);
  dump_header(s, "X-Container-Bytes-Used", bucket.size);
  dump_header(s, "X-Container-Bytes-Used-Actual", bucket.size_rounded);

  if (s->object.empty()) {
    auto swift_policy = \
      static_cast<RGWAccessControlPolicy_SWIFT*>(s->bucket_acl.get());
    std::string read_acl, write_acl;
    swift_policy->to_str(read_acl, write_acl);

    if (read_acl.size()) {
      dump_header(s, "X-Container-Read", read_acl);
    }
    if (write_acl.size()) {
      dump_header(s, "X-Container-Write", write_acl);
    }
    if (!s->bucket_info.placement_rule.name.empty()) {
      dump_header(s, "X-Storage-Policy", s->bucket_info.placement_rule.name);
    }
    dump_header(s, "X-Storage-Class", s->bucket_info.placement_rule.get_storage_class());

    /* Dump user-defined metadata items and generic attrs. */
    const size_t PREFIX_LEN = sizeof(RGW_ATTR_META_PREFIX) - 1;
    map<string, bufferlist>::iterator iter;
    for (iter = s->bucket_attrs.lower_bound(RGW_ATTR_PREFIX);
         iter != s->bucket_attrs.end();
         ++iter) {
      const char *name = iter->first.c_str();
      map<string, string>::const_iterator geniter = rgw_to_http_attrs.find(name);

      if (geniter != rgw_to_http_attrs.end()) {
        dump_header(s, geniter->second, iter->second);
      } else if (strncmp(name, RGW_ATTR_META_PREFIX, PREFIX_LEN) == 0) {
        dump_header_prefixed(s, "X-Container-Meta-",
                             camelcase_dash_http_attr(name + PREFIX_LEN),
                             iter->second);
      }
    }
  }

  /* Dump container versioning info. */
  if (! s->bucket_info.swift_ver_location.empty()) {
    dump_header(s, "X-Versions-Location",
                url_encode(s->bucket_info.swift_ver_location));
  }

  /* Dump quota headers. */
  if (quota.enabled) {
    if (quota.max_size >= 0) {
      dump_header(s, "X-Container-Meta-Quota-Bytes", quota.max_size);
    }

    if (quota.max_objects >= 0) {
      dump_header(s, "X-Container-Meta-Quota-Count", quota.max_objects);
    }
  }

  /* Dump Static Website headers. */
  if (! ws_conf.index_doc_suffix.empty()) {
    dump_header(s, "X-Container-Meta-Web-Index", ws_conf.index_doc_suffix);
  }

  if (! ws_conf.error_doc.empty()) {
    dump_header(s, "X-Container-Meta-Web-Error", ws_conf.error_doc);
  }

  if (! ws_conf.subdir_marker.empty()) {
    dump_header(s, "X-Container-Meta-Web-Directory-Type",
                ws_conf.subdir_marker);
  }

  if (! ws_conf.listing_css_doc.empty()) {
    dump_header(s, "X-Container-Meta-Web-Listings-CSS",
                ws_conf.listing_css_doc);
  }

  if (ws_conf.listing_enabled) {
    dump_header(s, "X-Container-Meta-Web-Listings", "true");
  }

  /* Dump bucket's modification time. Compliance with the Swift API really
   * needs that. */
  dump_last_modified(s, s->bucket_mtime);
}
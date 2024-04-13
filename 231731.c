void RGWListBucket_ObjStore_SWIFT::send_response()
{
  vector<rgw_bucket_dir_entry>::iterator iter = objs.begin();
  map<string, bool>::iterator pref_iter = common_prefixes.begin();

  dump_start(s);
  dump_container_metadata(s, bucket, bucket_quota,
                          s->bucket_info.website_conf);

  s->formatter->open_array_section_with_attrs("container",
					      FormatterAttrs("name",
							     s->bucket.name.c_str(),
							     NULL));

  while (iter != objs.end() || pref_iter != common_prefixes.end()) {
    bool do_pref = false;
    bool do_objs = false;
    rgw_obj_key key;
    if (iter != objs.end()) {
      key = iter->key;
    }
    if (pref_iter == common_prefixes.end())
      do_objs = true;
    else if (iter == objs.end())
      do_pref = true;
    else if (!key.empty() && key.name.compare(pref_iter->first) == 0) {
      do_objs = true;
      ++pref_iter;
    } else if (!key.empty() && key.name.compare(pref_iter->first) <= 0)
      do_objs = true;
    else
      do_pref = true;

    if (do_objs && (allow_unordered || marker.empty() || marker < key)) {
      if (key.name.compare(path) == 0)
        goto next;

      s->formatter->open_object_section("object");
      s->formatter->dump_string("name", key.name);
      s->formatter->dump_string("hash", iter->meta.etag);
      s->formatter->dump_int("bytes", iter->meta.accounted_size);
      if (!iter->meta.user_data.empty())
        s->formatter->dump_string("user_custom_data", iter->meta.user_data);
      string single_content_type = iter->meta.content_type;
      if (iter->meta.content_type.size()) {
        // content type might hold multiple values, just dump the last one
        ssize_t pos = iter->meta.content_type.rfind(',');
        if (pos > 0) {
          ++pos;
          while (single_content_type[pos] == ' ')
            ++pos;
          single_content_type = single_content_type.substr(pos);
        }
        s->formatter->dump_string("content_type", single_content_type);
      }
      dump_time(s, "last_modified", &iter->meta.mtime);
      s->formatter->close_section();
    }

    if (do_pref &&  (marker.empty() || pref_iter->first.compare(marker.name) > 0)) {
      const string& name = pref_iter->first;
      if (name.compare(delimiter) == 0)
        goto next;

      s->formatter->open_object_section_with_attrs("subdir", FormatterAttrs("name", name.c_str(), NULL));

      /* swift is a bit inconsistent here */
      switch (s->format) {
        case RGW_FORMAT_XML:
          s->formatter->dump_string("name", name);
          break;
        default:
          s->formatter->dump_string("subdir", name);
      }
      s->formatter->close_section();
    }
next:
    if (do_objs)
      ++iter;
    else
      ++pref_iter;
  }

  s->formatter->close_section();

  int64_t content_len = 0;
  if (! op_ret) {
    content_len = s->formatter->get_len();
    if (content_len == 0) {
      op_ret = STATUS_NO_CONTENT;
    }
  } else if (op_ret > 0) {
    op_ret = 0;
  }

  set_req_state_err(s, op_ret);
  dump_errno(s);
  end_header(s, this, NULL, content_len);
  if (op_ret < 0) {
    return;
  }

  rgw_flush_formatter_and_reset(s, s->formatter);
} // RGWListBucket_ObjStore_SWIFT::send_response
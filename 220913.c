void RGWListBucket_ObjStore_S3::send_response()
{
  if (op_ret < 0) {
    set_req_state_err(s, op_ret);
  }
  dump_errno(s);

  // Explicitly use chunked transfer encoding so that we can stream the result
  // to the user without having to wait for the full length of it.
  end_header(s, this, "application/xml", CHUNKED_TRANSFER_ENCODING);
  dump_start(s);
  if (op_ret < 0) {
    return;
  }
  if (list_versions) {
    send_versioned_response();
    return;
  }

  s->formatter->open_object_section_in_ns("ListBucketResult", XMLNS_AWS_S3);
  if (strcasecmp(encoding_type.c_str(), "url") == 0) {
    s->formatter->dump_string("EncodingType", "url");
    encode_key = true;
  }
  RGWListBucket_ObjStore_S3::send_common_response();
    if (op_ret >= 0) {
      vector<rgw_bucket_dir_entry>::iterator iter;
      for (iter = objs.begin(); iter != objs.end(); ++iter) {
        rgw_obj_key key(iter->key);
        s->formatter->open_array_section("Contents");
        if (encode_key) {
          string key_name;
          url_encode(key.name, key_name);
          s->formatter->dump_string("Key", key_name);
      } else {
          s->formatter->dump_string("Key", key.name);
      }
        dump_time(s, "LastModified", &iter->meta.mtime);
        s->formatter->dump_format("ETag", "\"%s\"", iter->meta.etag.c_str());
        s->formatter->dump_int("Size", iter->meta.accounted_size);
        auto& storage_class = rgw_placement_rule::get_canonical_storage_class(iter->meta.storage_class);
        s->formatter->dump_string("StorageClass", storage_class.c_str());
        dump_owner(s, iter->meta.owner, iter->meta.owner_display_name);
        if (s->system_request) {
          s->formatter->dump_string("RgwxTag", iter->tag);
      }
        if (iter->meta.appendable) {
          s->formatter->dump_string("Type", "Appendable");
      } else {
          s->formatter->dump_string("Type", "Normal");
      }
        s->formatter->close_section();
    }
  }
    s->formatter->dump_string("Marker", marker.name);
    if (is_truncated && !next_marker.empty()) {
      s->formatter->dump_string("NextMarker", next_marker.name);
    }
    s->formatter->close_section();
    rgw_flush_formatter_and_reset(s, s->formatter);
}
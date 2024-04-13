int RGWPutObj_ObjStore_SWIFT::get_params()
{
  if (s->has_bad_meta) {
    return -EINVAL;
  }

  if (!s->length) {
    const char *encoding = s->info.env->get("HTTP_TRANSFER_ENCODING");
    if (!encoding || strcmp(encoding, "chunked") != 0) {
      ldout(s->cct, 20) << "neither length nor chunked encoding" << dendl;
      return -ERR_LENGTH_REQUIRED;
    }

    chunked_upload = true;
  }

  supplied_etag = s->info.env->get("HTTP_ETAG");

  if (!s->generic_attrs.count(RGW_ATTR_CONTENT_TYPE)) {
    ldout(s->cct, 5) << "content type wasn't provided, trying to guess" << dendl;
    const char *suffix = strrchr(s->object.name.c_str(), '.');
    if (suffix) {
      suffix++;
      if (*suffix) {
	string suffix_str(suffix);
	const char *mime = rgw_find_mime_by_ext(suffix_str);
	if (mime) {
	  s->generic_attrs[RGW_ATTR_CONTENT_TYPE] = mime;
	}
      }
    }
  }

  policy.create_default(s->user->user_id, s->user->display_name);

  int r = get_delete_at_param(s, delete_at);
  if (r < 0) {
    ldout(s->cct, 5) << "ERROR: failed to get Delete-At param" << dendl;
    return r;
  }

  if (!s->cct->_conf->rgw_swift_custom_header.empty()) {
    string custom_header = s->cct->_conf->rgw_swift_custom_header;
    if (s->info.env->exists(custom_header.c_str())) {
      user_data = s->info.env->get(custom_header.c_str());
    }
  }

  dlo_manifest = s->info.env->get("HTTP_X_OBJECT_MANIFEST");
  bool exists;
  string multipart_manifest = s->info.args.get("multipart-manifest", &exists);
  if (exists) {
    if (multipart_manifest != "put") {
      ldout(s->cct, 5) << "invalid multipart-manifest http param: " << multipart_manifest << dendl;
      return -EINVAL;
    }

#define MAX_SLO_ENTRY_SIZE (1024 + 128) // 1024 - max obj name, 128 - enough extra for other info
    uint64_t max_len = s->cct->_conf->rgw_max_slo_entries * MAX_SLO_ENTRY_SIZE;
    
    slo_info = new RGWSLOInfo;
    
    int r = 0;
    std::tie(r, slo_info->raw_data) = rgw_rest_get_json_input_keep_data(s->cct, s, slo_info->entries, max_len);
    if (r < 0) {
      ldout(s->cct, 5) << "failed to read input for slo r=" << r << dendl;
      return r;
    }

    if ((int64_t)slo_info->entries.size() > s->cct->_conf->rgw_max_slo_entries) {
      ldout(s->cct, 5) << "too many entries in slo request: " << slo_info->entries.size() << dendl;
      return -EINVAL;
    }

    MD5 etag_sum;
    uint64_t total_size = 0;
    for (auto& entry : slo_info->entries) {
      etag_sum.Update((const unsigned char *)entry.etag.c_str(),
                      entry.etag.length());

      /* if size_bytes == 0, it should be replaced with the
       * real segment size (which could be 0);  this follows from the
       * fact that Swift requires all segments to exist, but permits
       * the size_bytes element to be omitted from the SLO manifest, see
       * https://docs.openstack.org/swift/latest/api/large_objects.html
       */
      r = update_slo_segment_size(entry);
      if (r < 0) {
	return r;
      }

      total_size += entry.size_bytes;

      ldout(s->cct, 20) << "slo_part: " << entry.path
                        << " size=" << entry.size_bytes
                        << " etag=" << entry.etag
                        << dendl;
    }
    complete_etag(etag_sum, &lo_etag);
    slo_info->total_size = total_size;

    ofs = slo_info->raw_data.length();
  }

  return RGWPutObj_ObjStore::get_params();
}
int RGWPutObj_ObjStore_S3::get_params()
{
  if (!s->length)
    return -ERR_LENGTH_REQUIRED;

  map<string, bufferlist> src_attrs;
  size_t pos;
  int ret;

  map_qs_metadata(s);

  RGWAccessControlPolicy_S3 s3policy(s->cct);
  ret = create_s3_policy(s, store, s3policy, s->owner);
  if (ret < 0)
    return ret;

  policy = s3policy;

  if_match = s->info.env->get("HTTP_IF_MATCH");
  if_nomatch = s->info.env->get("HTTP_IF_NONE_MATCH");
  copy_source = url_decode(s->info.env->get("HTTP_X_AMZ_COPY_SOURCE", ""));
  copy_source_range = s->info.env->get("HTTP_X_AMZ_COPY_SOURCE_RANGE");

  /* handle x-amz-copy-source */
  boost::string_view cs_view(copy_source);
  if (! cs_view.empty()) {
    if (cs_view[0] == '/')
      cs_view.remove_prefix(1);
    copy_source_bucket_name = cs_view.to_string();
    pos = copy_source_bucket_name.find("/");
    if (pos == std::string::npos) {
      ret = -EINVAL;
      ldout(s->cct, 5) << "x-amz-copy-source bad format" << dendl;
      return ret;
    }
    copy_source_object_name =
      copy_source_bucket_name.substr(pos + 1, copy_source_bucket_name.size());
    copy_source_bucket_name = copy_source_bucket_name.substr(0, pos);
#define VERSION_ID_STR "?versionId="
    pos = copy_source_object_name.find(VERSION_ID_STR);
    if (pos == std::string::npos) {
      copy_source_object_name = url_decode(copy_source_object_name);
    } else {
      copy_source_version_id =
	copy_source_object_name.substr(pos + sizeof(VERSION_ID_STR) - 1);
      copy_source_object_name =
	url_decode(copy_source_object_name.substr(0, pos));
    }
    pos = copy_source_bucket_name.find(":");
    if (pos == std::string::npos) {
       copy_source_tenant_name = s->src_tenant_name;
    } else {
       copy_source_tenant_name = copy_source_bucket_name.substr(0, pos);
       copy_source_bucket_name = copy_source_bucket_name.substr(pos + 1, copy_source_bucket_name.size());
       if (copy_source_bucket_name.empty()) {
         ret = -EINVAL;
         ldout(s->cct, 5) << "source bucket name is empty" << dendl;
         return ret;
       }
    }
    ret = store->get_bucket_info(*s->sysobj_ctx,
                                 copy_source_tenant_name,
                                 copy_source_bucket_name,
                                 copy_source_bucket_info,
                                 NULL, &src_attrs);
    if (ret < 0) {
      ldout(s->cct, 5) << __func__ << "(): get_bucket_info() returned ret=" << ret << dendl;
      return ret;
    }

    /* handle x-amz-copy-source-range */

    if (copy_source_range) {
      string range = copy_source_range;
      pos = range.find("=");
      if (pos == std::string::npos) {
        ret = -EINVAL;
        ldout(s->cct, 5) << "x-amz-copy-source-range bad format" << dendl;
        return ret;
      }
      range = range.substr(pos + 1);
      pos = range.find("-");
      if (pos == std::string::npos) {
        ret = -EINVAL;
        ldout(s->cct, 5) << "x-amz-copy-source-range bad format" << dendl;
        return ret;
      }
      string first = range.substr(0, pos);
      string last = range.substr(pos + 1);
      copy_source_range_fst = strtoull(first.c_str(), NULL, 10);
      copy_source_range_lst = strtoull(last.c_str(), NULL, 10);
    }

  } /* copy_source */

  /* handle object tagging */
  auto tag_str = s->info.env->get("HTTP_X_AMZ_TAGGING");
  if (tag_str){
    obj_tags = std::make_unique<RGWObjTags>();
    ret = obj_tags->set_from_string(tag_str);
    if (ret < 0){
      ldout(s->cct,0) << "setting obj tags failed with " << ret << dendl;
      if (ret == -ERR_INVALID_TAG){
        ret = -EINVAL; //s3 returns only -EINVAL for PUT requests
      }

      return ret;
    }
  }

  //handle object lock
  auto obj_lock_mode_str = s->info.env->get("HTTP_X_AMZ_OBJECT_LOCK_MODE");
  auto obj_lock_date_str = s->info.env->get("HTTP_X_AMZ_OBJECT_LOCK_RETAIN_UNTIL_DATE");
  auto obj_legal_hold_str = s->info.env->get("HTTP_X_AMZ_OBJECT_LOCK_LEGAL_HOLD");
  if (obj_lock_mode_str && obj_lock_date_str) {
    boost::optional<ceph::real_time> date = ceph::from_iso_8601(obj_lock_date_str);
    if (boost::none == date || ceph::real_clock::to_time_t(*date) <= ceph_clock_now()) {
        ret = -EINVAL;
        ldpp_dout(this,0) << "invalid x-amz-object-lock-retain-until-date value" << dendl;
        return ret;
    }
    if (strcmp(obj_lock_mode_str, "GOVERNANCE") != 0 && strcmp(obj_lock_mode_str, "COMPLIANCE") != 0) {
        ret = -EINVAL;
        ldpp_dout(this,0) << "invalid x-amz-object-lock-mode value" << dendl;
        return ret;
    }
    obj_retention = new RGWObjectRetention(obj_lock_mode_str, *date);
  } else if ((obj_lock_mode_str && !obj_lock_date_str) || (!obj_lock_mode_str && obj_lock_date_str)) {
    ret = -EINVAL;
    ldpp_dout(this,0) << "need both x-amz-object-lock-mode and x-amz-object-lock-retain-until-date " << dendl;
    return ret;
  }
  if (obj_legal_hold_str) {
    if (strcmp(obj_legal_hold_str, "ON") != 0 && strcmp(obj_legal_hold_str, "OFF") != 0) {
        ret = -EINVAL;
        ldpp_dout(this,0) << "invalid x-amz-object-lock-legal-hold value" << dendl;
        return ret;
    }
    obj_legal_hold = new RGWObjectLegalHold(obj_legal_hold_str);
  }
  if (!s->bucket_info.obj_lock_enabled() && (obj_retention || obj_legal_hold)) {
    ldpp_dout(this, 0) << "ERROR: object retention or legal hold can't be set if bucket object lock not configured" << dendl;
    ret = -ERR_INVALID_REQUEST;
    return ret;
  }
  multipart_upload_id = s->info.args.get("uploadId");
  multipart_part_str = s->info.args.get("partNumber");
  if (!multipart_part_str.empty()) {
    string err;
    multipart_part_num = strict_strtol(multipart_part_str.c_str(), 10, &err);
    if (!err.empty()) {
      ldpp_dout(s, 10) << "bad part number: " << multipart_part_str << ": " << err << dendl;
      return -EINVAL;
    }
  } else if (!multipart_upload_id.empty()) {
    ldpp_dout(s, 10) << "part number with no multipart upload id" << dendl;
    return -EINVAL;
  }

  append = s->info.args.exists("append");
  if (append) {
    string pos_str = s->info.args.get("position");
    if (pos_str.empty()) {
      return -EINVAL;
    } else {
      position = strtoull(pos_str.c_str(), NULL, 10);
    }
  }
  
  return RGWPutObj_ObjStore::get_params();
}
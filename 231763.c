int RGWPutObj_ObjStore_SWIFT::update_slo_segment_size(rgw_slo_entry& entry) {

  int r = 0;
  const string& path = entry.path;

  /* If the path starts with slashes, strip them all. */
  const size_t pos_init = path.find_first_not_of('/');

  if (pos_init == string::npos) {
    return -EINVAL;
  }

  const size_t pos_sep = path.find('/', pos_init);
  if (pos_sep == string::npos) {
    return -EINVAL;
  }

  string bucket_name = path.substr(pos_init, pos_sep - pos_init);
  string obj_name = path.substr(pos_sep + 1);

  rgw_bucket bucket;

  if (bucket_name.compare(s->bucket.name) != 0) {
    RGWBucketInfo bucket_info;
    map<string, bufferlist> bucket_attrs;
    auto obj_ctx = store->svc.sysobj->init_obj_ctx();
    r = store->get_bucket_info(obj_ctx, s->user->user_id.tenant,
			       bucket_name, bucket_info, nullptr,
			       &bucket_attrs);
    if (r < 0) {
      ldpp_dout(this, 0) << "could not get bucket info for bucket="
			 << bucket_name << dendl;
      return r;
    }
    bucket = bucket_info.bucket;
  } else {
    bucket = s->bucket;
  }

  /* fetch the stored size of the seg (or error if not valid) */
  rgw_obj_key slo_key(obj_name);
  rgw_obj slo_seg(bucket, slo_key);

  /* no prefetch */
  RGWObjectCtx obj_ctx(store);
  obj_ctx.set_atomic(slo_seg);

  RGWRados::Object op_target(store, s->bucket_info, obj_ctx, slo_seg);
  RGWRados::Object::Read read_op(&op_target);

  bool compressed;
  RGWCompressionInfo cs_info;
  map<std::string, buffer::list> attrs;
  uint64_t size_bytes{0};

  read_op.params.attrs = &attrs;
  read_op.params.obj_size = &size_bytes;

  r = read_op.prepare();
  if (r < 0) {
    return r;
  }

  r = rgw_compression_info_from_attrset(attrs, compressed, cs_info);
  if (r < 0) {
    return -EIO;
  }

  if (compressed) {
    size_bytes = cs_info.orig_size;
  }

  /* "When the PUT operation sees the multipart-manifest=put query
   * parameter, it reads the request body and verifies that each
   * segment object exists and that the sizes and ETags match. If
   * there is a mismatch, the PUT operation fails."
   */
  if (entry.size_bytes &&
      (entry.size_bytes != size_bytes)) {
    return -EINVAL;
  }

  entry.size_bytes = size_bytes;

  return 0;
} /* RGWPutObj_ObjStore_SWIFT::update_slo_segment_sizes */
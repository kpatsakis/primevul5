bool RGWHandler_REST_S3Website::web_dir() const {
  std::string subdir_name = url_decode(s->object.name);

  if (subdir_name.empty()) {
    return false;
  } else if (subdir_name.back() == '/') {
    subdir_name.pop_back();
  }

  rgw_obj obj(s->bucket, subdir_name);

  RGWObjectCtx& obj_ctx = *static_cast<RGWObjectCtx *>(s->obj_ctx);
  obj_ctx.set_atomic(obj);
  obj_ctx.set_prefetch_data(obj);

  RGWObjState* state = nullptr;
  if (store->get_obj_state(&obj_ctx, s->bucket_info, obj, &state, false) < 0) {
    return false;
  }
  if (! state->exists) {
    return false;
  }
  return state->exists;
}
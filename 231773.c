bool RGWSwiftWebsiteHandler::is_index_present(const std::string& index)
{
  rgw_obj obj(s->bucket, index);

  RGWObjectCtx& obj_ctx = *static_cast<RGWObjectCtx *>(s->obj_ctx);
  obj_ctx.set_atomic(obj);
  obj_ctx.set_prefetch_data(obj);

  RGWObjState* state = nullptr;
  if (store->get_obj_state(&obj_ctx, s->bucket_info, obj, &state, false) < 0) {
    return false;
  }

  /* A nonexistent object cannot be a considered as a viable index. We will
   * try to list the bucket or - if this is impossible - return an error. */
  return state->exists;
}
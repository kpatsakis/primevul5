int RGWHandler_REST_SWIFT::init(RGWRados* store, struct req_state* s,
				rgw::io::BasicClient *cio)
{
  struct req_init_state *t = &s->init_state;

  s->dialect = "swift";

  std::string copy_source = s->info.env->get("HTTP_X_COPY_FROM", "");
  if (! copy_source.empty()) {
    bool result = RGWCopyObj::parse_copy_location(copy_source, t->src_bucket,
						  s->src_object);
    if (!result)
      return -ERR_BAD_URL;
  }

  if (s->op == OP_COPY) {
    std::string req_dest = s->info.env->get("HTTP_DESTINATION", "");
    if (req_dest.empty())
      return -ERR_BAD_URL;

    std::string dest_bucket_name;
    rgw_obj_key dest_obj_key;
    bool result =
      RGWCopyObj::parse_copy_location(req_dest, dest_bucket_name,
				      dest_obj_key);
    if (!result)
       return -ERR_BAD_URL;

    std::string dest_object = dest_obj_key.name;

    /* convert COPY operation into PUT */
    t->src_bucket = t->url_bucket;
    s->src_object = s->object;
    t->url_bucket = dest_bucket_name;
    s->object = rgw_obj_key(dest_object);
    s->op = OP_PUT;
  }

  s->info.storage_class = s->info.env->get("HTTP_X_OBJECT_STORAGE_CLASS", "");

  return RGWHandler_REST::init(store, s, cio);
}
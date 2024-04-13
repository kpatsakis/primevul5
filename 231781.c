static int get_swift_container_settings(req_state * const s,
                                        RGWRados * const store,
                                        RGWAccessControlPolicy * const policy,
                                        bool * const has_policy,
                                        uint32_t * rw_mask,
                                        RGWCORSConfiguration * const cors_config,
                                        bool * const has_cors)
{
  const char * const read_list = s->info.env->get("HTTP_X_CONTAINER_READ");
  const char * const write_list = s->info.env->get("HTTP_X_CONTAINER_WRITE");

  *has_policy = false;

  if (read_list || write_list) {
    RGWAccessControlPolicy_SWIFT swift_policy(s->cct);
    const auto r = swift_policy.create(store,
                                       s->user->user_id,
                                       s->user->display_name,
                                       read_list,
                                       write_list,
                                       *rw_mask);
    if (r < 0) {
      return r;
    }

    *policy = swift_policy;
    *has_policy = true;
  }

  *has_cors = false;

  /*Check and update CORS configuration*/
  const char *allow_origins = s->info.env->get("HTTP_X_CONTAINER_META_ACCESS_CONTROL_ALLOW_ORIGIN");
  const char *allow_headers = s->info.env->get("HTTP_X_CONTAINER_META_ACCESS_CONTROL_ALLOW_HEADERS");
  const char *expose_headers = s->info.env->get("HTTP_X_CONTAINER_META_ACCESS_CONTROL_EXPOSE_HEADERS");
  const char *max_age = s->info.env->get("HTTP_X_CONTAINER_META_ACCESS_CONTROL_MAX_AGE");
  if (allow_origins) {
    RGWCORSConfiguration_SWIFT *swift_cors = new RGWCORSConfiguration_SWIFT;
    int r = swift_cors->create_update(allow_origins, allow_headers, expose_headers, max_age);
    if (r < 0) {
      dout(0) << "Error creating/updating the cors configuration" << dendl;
      delete swift_cors;
      return r;
    }
    *has_cors = true;
    *cors_config = *swift_cors;
    cors_config->dump();
    delete swift_cors;
  }

  return 0;
}
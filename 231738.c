static int get_delete_at_param(req_state *s, boost::optional<real_time> &delete_at)
{
  /* Handle Swift object expiration. */
  real_time delat_proposal;
  string x_delete = s->info.env->get("HTTP_X_DELETE_AFTER", "");

  if (x_delete.empty()) {
    x_delete = s->info.env->get("HTTP_X_DELETE_AT", "");
  } else {
    /* X-Delete-After HTTP is present. It means we need add its value
     * to the current time. */
    delat_proposal = real_clock::now();
  }

  if (x_delete.empty()) {
    delete_at = boost::none;
    if (s->info.env->exists("HTTP_X_REMOVE_DELETE_AT")) {
      delete_at = boost::in_place(real_time());
    }
    return 0;
  }
  string err;
  long ts = strict_strtoll(x_delete.c_str(), 10, &err);

  if (!err.empty()) {
    return -EINVAL;
  }

  delat_proposal += make_timespan(ts);
  if (delat_proposal < real_clock::now()) {
    return -EINVAL;
  }

  delete_at = delat_proposal;

  return 0;
}
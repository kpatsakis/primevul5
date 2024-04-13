static int drop_privileges(CephContext *ctx)
{
  uid_t uid = ctx->get_set_uid();
  gid_t gid = ctx->get_set_gid();
  std::string uid_string = ctx->get_set_uid_string();
  std::string gid_string = ctx->get_set_gid_string();
  if (gid && setgid(gid) != 0) {
    int err = errno;
    ldout(ctx, -1) << "unable to setgid " << gid << ": " << cpp_strerror(err) << dendl;
    return -err;
  }
  if (uid && setuid(uid) != 0) {
    int err = errno;
    ldout(ctx, -1) << "unable to setuid " << uid << ": " << cpp_strerror(err) << dendl;
    return -err;
  }
  if (uid && gid) {
    ldout(ctx, 0) << "set uid:gid to " << uid << ":" << gid
                  << " (" << uid_string << ":" << gid_string << ")" << dendl;
  }
  return 0;
}
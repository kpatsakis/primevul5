fchown_or_chown (int fd, const char *name, uid_t uid, uid_t gid)
{
  if (HAVE_FCHOWN && fd != -1)
    return fchown (fd, uid, gid);
  else
    return chown (name, uid, gid);
}
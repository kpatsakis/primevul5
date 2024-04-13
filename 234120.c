fchmod_or_chmod (int fd, const char *name, mode_t mode)
{
  if (HAVE_FCHMOD && fd != -1)
    return fchmod (fd, mode);
  else
    return chmod (name, mode);
}
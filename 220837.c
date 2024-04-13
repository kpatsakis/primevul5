repodata_chk2str(Repodata *data, Id type, const unsigned char *buf)
{
  int l;

  if (!(l = solv_chksum_len(type)))
    return "";
  return pool_bin2hex(data->repo->pool, buf, l);
}
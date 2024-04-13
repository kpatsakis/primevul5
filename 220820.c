repodata_set_checksum(Repodata *data, Id solvid, Id keyname, Id type,
		      const char *str)
{
  unsigned char buf[64];
  int l;

  if (!(l = solv_chksum_len(type)))
    return;
  if (l > sizeof(buf) || solv_hex2bin(&str, buf, l) != l)
    return;
  repodata_set_bin_checksum(data, solvid, keyname, type, buf);
}
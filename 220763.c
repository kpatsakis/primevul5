repodata_set_strn(Repodata *data, Id solvid, Id keyname, const char *str, int l)
{
  if (!str[l])
    repodata_set_str(data, solvid, keyname, str);
  else
    {
      char *s = solv_strdup(str);
      s[l] = 0;
      repodata_set_str(data, solvid, keyname, s);
      free(s);
    }
}
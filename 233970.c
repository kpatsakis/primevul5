url_set_file (struct url *url, const char *newfile)
{
  xfree (url->file);
  url->file = xstrdup (newfile);
  sync_path (url);
}
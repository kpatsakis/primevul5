bool check_valid_path(const char *path, size_t len)
{
  size_t prefix= my_strcspn(files_charset_info, path, path + len, FN_DIRSEP);
  return  prefix < len;
}
archive_mstring_copy_utf8(struct archive_mstring *aes, const char *utf8)
{
  if (utf8 == NULL) {
    aes->aes_set = 0;
    return (0);
  }
  aes->aes_set = AES_SET_UTF8;
  archive_string_empty(&(aes->aes_mbs));
  archive_string_empty(&(aes->aes_wcs));
  archive_strncpy(&(aes->aes_utf8), utf8, strlen(utf8));
  return (int)strlen(utf8);
}
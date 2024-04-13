_hivex_get_iconv (hive_h *h, recode_type t)
{
  gl_lock_lock (h->iconv_cache[t].mutex);
  if (h->iconv_cache[t].handle == NULL) {
    if (t == utf8_to_latin1)
      h->iconv_cache[t].handle = iconv_open ("LATIN1", "UTF-8");
    else if (t == latin1_to_utf8)
      h->iconv_cache[t].handle = iconv_open ("UTF-8", "LATIN1");
    else if (t == utf8_to_utf16le)
      h->iconv_cache[t].handle = iconv_open ("UTF-16LE", "UTF-8");
    else if (t == utf16le_to_utf8)
      h->iconv_cache[t].handle = iconv_open ("UTF-8", "UTF-16LE");
  } else {
    /* reinitialize iconv context */
    iconv (h->iconv_cache[t].handle, NULL, 0, NULL, 0);
  }
  return h->iconv_cache[t].handle;
}
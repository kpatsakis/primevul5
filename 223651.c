_hivex_release_iconv (hive_h *h, recode_type t)
{
  gl_lock_unlock (h->iconv_cache[t].mutex);
}
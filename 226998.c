static void imap_hcache_namer(const char *path, struct Buffer *dest)
{
  mutt_buffer_printf(dest, "%s.hcache", path);
}
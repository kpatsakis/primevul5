int imap_expand_path(struct Buffer *buf)
{
  mutt_buffer_alloc(buf, PATH_MAX);
  return imap_path_canon(buf->data, PATH_MAX);
}
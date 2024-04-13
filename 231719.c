void RGWCopyObj_ObjStore_SWIFT::dump_copy_info()
{
  /* Dump X-Copied-From. */
  dump_header(s, "X-Copied-From", url_encode(src_bucket.name) +
              "/" + url_encode(src_object.name));

  /* Dump X-Copied-From-Account. */
  /* XXX tenant */
  dump_header(s, "X-Copied-From-Account", url_encode(s->user->user_id.id));

  /* Dump X-Copied-From-Last-Modified. */
  dump_time_header(s, "X-Copied-From-Last-Modified", src_mtime);
}
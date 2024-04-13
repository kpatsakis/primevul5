void RGWListBuckets_ObjStore_SWIFT::dump_bucket_entry(const RGWBucketEnt& obj)
{
  s->formatter->open_object_section("container");
  s->formatter->dump_string("name", obj.bucket.name);

  if (need_stats) {
    s->formatter->dump_int("count", obj.count);
    s->formatter->dump_int("bytes", obj.size);
  }

  s->formatter->close_section();

  if (! s->cct->_conf->rgw_swift_enforce_content_length) {
    rgw_flush_formatter(s, s->formatter);
  }
}
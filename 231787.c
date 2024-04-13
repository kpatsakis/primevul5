void RGWListBuckets_ObjStore_SWIFT::send_response_data_reversed(RGWUserBuckets& buckets)
{
  if (! sent_data) {
    return;
  }

  /* Take care of the prefix parameter of Swift API. There is no business
   * in applying the filter earlier as we really need to go through all
   * entries regardless of it (the headers like X-Account-Container-Count
   * aren't affected by specifying prefix). */
  std::map<std::string, RGWBucketEnt>& m = buckets.get_buckets();

  auto iter = m.rbegin();
  for (/* initialized above */;
       iter != m.rend() && !boost::algorithm::starts_with(iter->first, prefix);
       ++iter) {
    /* NOP */;
  }

  for (/* iter carried */;
       iter != m.rend() && boost::algorithm::starts_with(iter->first, prefix);
       ++iter) {
    dump_bucket_entry(iter->second);
  }
}
void RGWListBuckets_ObjStore_SWIFT::send_response_data(RGWUserBuckets& buckets)
{
  if (! sent_data) {
    return;
  }

  /* Take care of the prefix parameter of Swift API. There is no business
   * in applying the filter earlier as we really need to go through all
   * entries regardless of it (the headers like X-Account-Container-Count
   * aren't affected by specifying prefix). */
  const std::map<std::string, RGWBucketEnt>& m = buckets.get_buckets();
  for (auto iter = m.lower_bound(prefix);
       iter != m.end() && boost::algorithm::starts_with(iter->first, prefix);
       ++iter) {
    dump_bucket_entry(iter->second);
  }
}
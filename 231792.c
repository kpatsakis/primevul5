void RGWListBuckets_ObjStore_SWIFT::handle_listing_chunk(RGWUserBuckets&& buckets)
{
  if (wants_reversed) {
    /* Just store in the reversal buffer. Its content will be handled later,
     * in send_response_end(). */
    reverse_buffer.emplace(std::begin(reverse_buffer), std::move(buckets));
  } else {
    return send_response_data(buckets);
  }
}
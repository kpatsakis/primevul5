int RGWHandler_REST_SWIFT::validate_bucket_name(const string& bucket)
{
  const size_t len = bucket.size();

  if (len > MAX_BUCKET_NAME_LEN) {
    /* Bucket Name too long. Generate custom error message and bind it
     * to an R-value reference. */
    const auto msg = boost::str(
      boost::format("Container name length of %lld longer than %lld")
        % len % int(MAX_BUCKET_NAME_LEN));
    set_req_state_err(s, ERR_INVALID_BUCKET_NAME, msg);
    return -ERR_INVALID_BUCKET_NAME;
  }

  const auto ret = RGWHandler_REST::validate_bucket_name(bucket);
  if (ret < 0) {
    return ret;
  }

  if (len == 0)
    return 0;

  if (bucket[0] == '.')
    return -ERR_INVALID_BUCKET_NAME;

  if (check_utf8(bucket.c_str(), len))
    return -ERR_INVALID_UTF8;

  const char *s = bucket.c_str();

  for (size_t i = 0; i < len; ++i, ++s) {
    if (*(unsigned char *)s == 0xff)
      return -ERR_INVALID_BUCKET_NAME;
    if (*(unsigned char *)s == '/')
      return -ERR_INVALID_BUCKET_NAME;
  }

  return 0;
}
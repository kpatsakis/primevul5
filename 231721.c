bool RGWFormPost::is_non_expired()
{
  std::string expires = get_part_str(ctrl_parts, "expires", "0");

  std::string err;
  const uint64_t expires_timestamp =
    static_cast<uint64_t>(strict_strtoll(expires.c_str(), 10, &err));

  if (! err.empty()) {
    dout(5) << "failed to parse FormPost's expires: " << err << dendl;
    return false;
  }

  const utime_t now = ceph_clock_now();
  if (expires_timestamp <= static_cast<uint64_t>(now.sec())) {
    dout(5) << "FormPost form expired: "
            << expires_timestamp << " <= " << now.sec() << dendl;
    return false;
  }

  return true;
}
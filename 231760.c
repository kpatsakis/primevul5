bool RGWInfo_ObjStore_SWIFT::is_expired(const std::string& expires, CephContext* cct)
{
  string err;
  const utime_t now = ceph_clock_now();
  const uint64_t expiration = (uint64_t)strict_strtoll(expires.c_str(),
                                                       10, &err);
  if (!err.empty()) {
    ldout(cct, 5) << "failed to parse siginfo_expires: " << err << dendl;
    return true;
  }

  if (expiration <= (uint64_t)now.sec()) {
    ldout(cct, 5) << "siginfo expired: " << expiration << " <= " << now.sec() << dendl;
    return true;
  }

  return false;
}
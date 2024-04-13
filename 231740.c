int RGWListBuckets_ObjStore_SWIFT::get_params()
{
  prefix = s->info.args.get("prefix");
  marker = s->info.args.get("marker");
  end_marker = s->info.args.get("end_marker");
  wants_reversed = s->info.args.exists("reverse");

  if (wants_reversed) {
    std::swap(marker, end_marker);
  }

  std::string limit_str = s->info.args.get("limit");
  if (!limit_str.empty()) {
    std::string err;
    long l = strict_strtol(limit_str.c_str(), 10, &err);
    if (!err.empty()) {
      return -EINVAL;
    }

    if (l > (long)limit_max || l < 0) {
      return -ERR_PRECONDITION_FAILED;
    }

    limit = (uint64_t)l;
  }

  if (s->cct->_conf->rgw_swift_need_stats) {
    bool stats, exists;
    int r = s->info.args.get_bool("stats", &stats, &exists);

    if (r < 0) {
      return r;
    }

    if (exists) {
      need_stats = stats;
    }
  } else {
    need_stats = false;
  }

  return 0;
}
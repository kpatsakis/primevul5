int RGWListBucket_ObjStore_SWIFT::get_params()
{
  prefix = s->info.args.get("prefix");
  marker = s->info.args.get("marker");
  end_marker = s->info.args.get("end_marker");
  max_keys = s->info.args.get("limit");

  // non-standard
  s->info.args.get_bool("allow_unordered", &allow_unordered, false);

  delimiter = s->info.args.get("delimiter");

  op_ret = parse_max_keys();
  if (op_ret < 0) {
    return op_ret;
  }
  // S3 behavior is to silently cap the max-keys.
  // Swift behavior is to abort.
  if (max > default_max)
    return -ERR_PRECONDITION_FAILED;

  string path_args;
  if (s->info.args.exists("path")) { // should handle empty path
    path_args = s->info.args.get("path");
    if (!delimiter.empty() || !prefix.empty()) {
      return -EINVAL;
    }
    prefix = path_args;
    delimiter="/";

    path = prefix;
    if (path.size() && path[path.size() - 1] != '/')
      path.append("/");

    int len = prefix.size();
    int delim_size = delimiter.size();

    if (len >= delim_size) {
      if (prefix.substr(len - delim_size).compare(delimiter) != 0)
        prefix.append(delimiter);
    }
  }

  return 0;
}
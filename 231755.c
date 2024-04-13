std::size_t RGWFormPost::get_max_file_size() /*const*/
{
  std::string max_str = get_part_str(ctrl_parts, "max_file_size", "0");

  std::string err;
  const std::size_t max_file_size =
    static_cast<uint64_t>(strict_strtoll(max_str.c_str(), 10, &err));

  if (! err.empty()) {
    ldout(s->cct, 5) << "failed to parse FormPost's max_file_size: " << err
                     << dendl;
    return 0;
  }

  return max_file_size;
}
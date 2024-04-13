int RGWBulkDelete_ObjStore_SWIFT::get_data(
  list<RGWBulkDelete::acct_path_t>& items, bool * const is_truncated)
{
  constexpr size_t MAX_LINE_SIZE = 2048;

  RGWClientIOStreamBuf ciosb(static_cast<RGWRestfulIO&>(*(s->cio)),
			     size_t(s->cct->_conf->rgw_max_chunk_size));
  istream cioin(&ciosb);

  char buf[MAX_LINE_SIZE];
  while (cioin.getline(buf, sizeof(buf))) {
    string path_str(buf);

    ldout(s->cct, 20) << "extracted Bulk Delete entry: " << path_str << dendl;

    RGWBulkDelete::acct_path_t path;

    /* We need to skip all slashes at the beginning in order to preserve
     * compliance with Swift. */
    const size_t start_pos = path_str.find_first_not_of('/');

    if (string::npos != start_pos) {
      /* Seperator is the first slash after the leading ones. */
      const size_t sep_pos = path_str.find('/', start_pos);

      if (string::npos != sep_pos) {
        path.bucket_name = url_decode(path_str.substr(start_pos,
                                                      sep_pos - start_pos));
        path.obj_key = url_decode(path_str.substr(sep_pos + 1));
      } else {
        /* It's guaranteed here that bucket name is at least one character
         * long and is different than slash. */
        path.bucket_name = url_decode(path_str.substr(start_pos));
      }

      items.push_back(path);
    }

    if (items.size() == MAX_CHUNK_ENTRIES) {
      *is_truncated = true;
      return 0;
    }
  }

  *is_truncated = false;
  return 0;
}
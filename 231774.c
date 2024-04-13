RGWBulkUploadOp_ObjStore_SWIFT::create_stream()
{
  class SwiftStreamGetter : public StreamGetter {
    const size_t conlen;
    size_t curpos;
    req_state* const s;

  public:
    SwiftStreamGetter(req_state* const s, const size_t conlen)
      : conlen(conlen),
        curpos(0),
        s(s) {
    }

    ssize_t get_at_most(size_t want, ceph::bufferlist& dst) override {
      /* maximum requested by a caller */
      /* data provided by client */
      /* RadosGW's limit. */
      const size_t max_chunk_size = \
        static_cast<size_t>(s->cct->_conf->rgw_max_chunk_size);
      const size_t max_to_read = std::min({ want, conlen - curpos, max_chunk_size });

      ldout(s->cct, 20) << "bulk_upload: get_at_most max_to_read="
                        << max_to_read
                        << ", dst.c_str()=" << reinterpret_cast<intptr_t>(dst.c_str()) << dendl;

      bufferptr bp(max_to_read);
      const auto read_len = recv_body(s, bp.c_str(), max_to_read);
      dst.append(bp, 0, read_len);
      //const auto read_len = recv_body(s, dst.c_str(), max_to_read);
      if (read_len < 0) {
        return read_len;
      }

      curpos += read_len;
      return curpos > s->cct->_conf->rgw_max_put_size ? -ERR_TOO_LARGE
                                                      : read_len;
    }

    ssize_t get_exactly(size_t want, ceph::bufferlist& dst) override {
      ldout(s->cct, 20) << "bulk_upload: get_exactly want=" << want << dendl;

      /* FIXME: do this in a loop. */
      const auto ret = get_at_most(want, dst);
      ldout(s->cct, 20) << "bulk_upload: get_exactly ret=" << ret << dendl;
      if (ret < 0) {
        return ret;
      } else if (static_cast<size_t>(ret) != want) {
        return -EINVAL;
      } else {
        return want;
      }
    }
  };

  if (! s->length) {
    op_ret = -EINVAL;
    return nullptr;
  } else {
    ldout(s->cct, 20) << "bulk upload: create_stream for length="
                      << s->length << dendl;

    const size_t conlen = atoll(s->length);
    return std::unique_ptr<SwiftStreamGetter>(new SwiftStreamGetter(s, conlen));
  }
}
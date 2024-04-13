void RGWCopyObj_ObjStore_SWIFT::send_partial_response(off_t ofs)
{
  if (! sent_header) {
    if (! op_ret)
      op_ret = STATUS_CREATED;
    set_req_state_err(s, op_ret);
    dump_errno(s);
    end_header(s, this);

    /* Send progress information. Note that this diverge from the original swift
     * spec. We do this in order to keep connection alive.
     */
    if (op_ret == 0) {
      s->formatter->open_array_section("progress");
    }
    sent_header = true;
  } else {
    s->formatter->dump_int("ofs", (uint64_t)ofs);
  }
  rgw_flush_formatter(s, s->formatter);
}
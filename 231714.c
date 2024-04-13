void RGWBulkDelete_ObjStore_SWIFT::send_response()
{
  set_req_state_err(s, op_ret);
  dump_errno(s);
  end_header(s, this /* RGWOp */, nullptr /* contype */,
             CHUNKED_TRANSFER_ENCODING);

  bulkdelete_respond(deleter->get_num_deleted(),
                     deleter->get_num_unfound(),
                     deleter->get_failures(),
                     s->prot_flags,
                     *s->formatter);
  rgw_flush_formatter_and_reset(s, s->formatter);
}
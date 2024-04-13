void RGWDeleteObj_ObjStore_SWIFT::send_response()
{
  int r = op_ret;

  if (multipart_delete) {
    r = 0;
  } else if(!r) {
    r = STATUS_NO_CONTENT;
  }

  set_req_state_err(s, r);
  dump_errno(s);

  if (multipart_delete) {
    end_header(s, this /* RGWOp */, nullptr /* contype */,
               CHUNKED_TRANSFER_ENCODING);

    if (deleter) {
      bulkdelete_respond(deleter->get_num_deleted(),
                         deleter->get_num_unfound(),
                         deleter->get_failures(),
                         s->prot_flags,
                         *s->formatter);
    } else if (-ENOENT == op_ret) {
      bulkdelete_respond(0, 1, {}, s->prot_flags, *s->formatter);
    } else {
      RGWBulkDelete::acct_path_t path;
      path.bucket_name = s->bucket_name;
      path.obj_key = s->object;

      RGWBulkDelete::fail_desc_t fail_desc;
      fail_desc.err = op_ret;
      fail_desc.path = path;

      bulkdelete_respond(0, 0, { fail_desc }, s->prot_flags, *s->formatter);
    }
  } else {
    end_header(s, this);
  }

  rgw_flush_formatter_and_reset(s, s->formatter);

}
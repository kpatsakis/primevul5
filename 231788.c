void RGWBulkUploadOp_ObjStore_SWIFT::send_response()
{
  set_req_state_err(s, op_ret);
  dump_errno(s);
  end_header(s, this /* RGWOp */, nullptr /* contype */,
             CHUNKED_TRANSFER_ENCODING);
  rgw_flush_formatter_and_reset(s, s->formatter);

  s->formatter->open_object_section("delete");

  std::string resp_status;
  std::string resp_body;

  if (! failures.empty()) {
    rgw_err err;

    const auto last_err = { failures.back().err };
    if (boost::algorithm::contains(last_err, terminal_errors)) {
      /* The terminal errors are affecting the status of the whole upload. */
      set_req_state_err(err, failures.back().err, s->prot_flags);
    } else {
      set_req_state_err(err, ERR_INVALID_REQUEST, s->prot_flags);
    }

    dump_errno(err, resp_status);
  } else if (0 == num_created && failures.empty()) {
    /* Nothing created, nothing failed. This means the archive contained no
     * entity we could understand (regular file or directory). We need to
     * send 400 Bad Request to an HTTP client in the internal status field. */
    dump_errno(400, resp_status);
    resp_body = "Invalid Tar File: No Valid Files";
  } else {
    /* 200 OK */
    dump_errno(201, resp_status);
  }

  encode_json("Number Files Created", num_created, s->formatter);
  encode_json("Response Body", resp_body, s->formatter);
  encode_json("Response Status", resp_status, s->formatter);

  s->formatter->open_array_section("Errors");
  for (const auto& fail_desc : failures) {
    s->formatter->open_array_section("object");

    encode_json("Name", fail_desc.path, s->formatter);

    rgw_err err;
    set_req_state_err(err, fail_desc.err, s->prot_flags);
    std::string status;
    dump_errno(err, status);
    encode_json("Status", status, s->formatter);

    s->formatter->close_section();
  }
  s->formatter->close_section();

  s->formatter->close_section();
  rgw_flush_formatter_and_reset(s, s->formatter);
}
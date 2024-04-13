static void bulkdelete_respond(const unsigned num_deleted,
                               const unsigned int num_unfound,
                               const std::list<RGWBulkDelete::fail_desc_t>& failures,
                               const int prot_flags,                  /* in  */
                               ceph::Formatter& formatter)            /* out */
{
  formatter.open_object_section("delete");

  string resp_status;
  string resp_body;

  if (!failures.empty()) {
    int reason = ERR_INVALID_REQUEST;
    for (const auto fail_desc : failures) {
      if (-ENOENT != fail_desc.err && -EACCES != fail_desc.err) {
        reason = fail_desc.err;
      }
    }
    rgw_err err;
    set_req_state_err(err, reason, prot_flags);
    dump_errno(err, resp_status);
  } else if (0 == num_deleted && 0 == num_unfound) {
    /* 400 Bad Request */
    dump_errno(400, resp_status);
    resp_body = "Invalid bulk delete.";
  } else {
    /* 200 OK */
    dump_errno(200, resp_status);
  }

  encode_json("Number Deleted", num_deleted, &formatter);
  encode_json("Number Not Found", num_unfound, &formatter);
  encode_json("Response Body", resp_body, &formatter);
  encode_json("Response Status", resp_status, &formatter);

  formatter.open_array_section("Errors");
  for (const auto fail_desc : failures) {
    formatter.open_array_section("object");

    stringstream ss_name;
    ss_name << fail_desc.path;
    encode_json("Name", ss_name.str(), &formatter);

    rgw_err err;
    set_req_state_err(err, fail_desc.err, prot_flags);
    string status;
    dump_errno(err, status);
    encode_json("Status", status, &formatter);
    formatter.close_section();
  }
  formatter.close_section();

  formatter.close_section();
}
void RGWGetHealthCheck_ObjStore_SWIFT::send_response()
{
  set_req_state_err(s, op_ret);
  dump_errno(s);
  end_header(s, this, "application/xml");

  if (op_ret) {
    static constexpr char DISABLED[] = "DISABLED BY FILE";
    dump_body(s, DISABLED, strlen(DISABLED));
  }
}
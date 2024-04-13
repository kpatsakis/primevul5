void RGWFormPost::send_response()
{
  std::string redirect = get_part_str(ctrl_parts, "redirect");
  if (! redirect.empty()) {
    op_ret = STATUS_REDIRECT;
  }

  set_req_state_err(s, op_ret);
  s->err.err_code = err_msg;
  dump_errno(s);
  if (! redirect.empty()) {
    dump_redirect(s, redirect);
  }
  end_header(s, this);
}
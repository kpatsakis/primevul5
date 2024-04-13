void RGWStatAccount_ObjStore_SWIFT::send_response()
{
  if (op_ret >= 0) {
    op_ret = STATUS_NO_CONTENT;
    dump_account_metadata(s,
            global_stats,
            policies_stats,
            attrs,
            user_quota,
            static_cast<RGWAccessControlPolicy_SWIFTAcct&>(*s->user_acl));
  }

  set_req_state_err(s, op_ret);
  dump_errno(s);

  end_header(s, NULL, NULL, 0,  true);

  dump_start(s);
}
static int get_swift_account_settings(req_state * const s,
                                      RGWRados * const store,
                                      RGWAccessControlPolicy_SWIFTAcct * const policy,
                                      bool * const has_policy)
{
  *has_policy = false;

  const char * const acl_attr = s->info.env->get("HTTP_X_ACCOUNT_ACCESS_CONTROL");
  if (acl_attr) {
    RGWAccessControlPolicy_SWIFTAcct swift_acct_policy(s->cct);
    const bool r = swift_acct_policy.create(store,
                                     s->user->user_id,
                                     s->user->display_name,
                                     string(acl_attr));
    if (r != true) {
      return -EINVAL;
    }

    *policy = swift_acct_policy;
    *has_policy = true;
  }

  return 0;
}
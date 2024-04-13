int RGWPutMetadataAccount_ObjStore_SWIFT::get_params()
{
  if (s->has_bad_meta) {
    return -EINVAL;
  }

  int ret = get_swift_account_settings(s,
                                       store,
                                       // FIXME: we need to carry unique_ptr in generic class
                                       // and allocate appropriate ACL class in the ctor
                                       static_cast<RGWAccessControlPolicy_SWIFTAcct *>(&policy),
                                       &has_policy);
  if (ret < 0) {
    return ret;
  }

  get_rmattrs_from_headers(s, ACCT_PUT_ATTR_PREFIX, ACCT_REMOVE_ATTR_PREFIX,
			   rmattr_names);
  return 0;
}
rgw::auth::s3::STSEngine::authenticate(
  const DoutPrefixProvider* dpp,
  const boost::string_view& _access_key_id,
  const boost::string_view& signature,
  const boost::string_view& session_token,
  const string_to_sign_t& string_to_sign,
  const signature_factory_t& signature_factory,
  const completer_factory_t& completer_factory,
  const req_state* const s) const
{
  if (! s->info.args.exists("X-Amz-Security-Token") &&
      ! s->info.env->exists("HTTP_X_AMZ_SECURITY_TOKEN")) {
    return result_t::deny();
  }

  STS::SessionToken token;
  if (int ret = get_session_token(session_token, token); ret < 0) {
    return result_t::reject(ret);
  }
  //Authentication
  //Check if access key is not the same passed in by client
  if (token.access_key_id != _access_key_id) {
    ldpp_dout(dpp, 0) << "Invalid access key" << dendl;
    return result_t::reject(-EPERM);
  }
  //Check if the token has expired
  if (! token.expiration.empty()) {
    std::string expiration = token.expiration;
    if (! expiration.empty()) {
      boost::optional<real_clock::time_point> exp = ceph::from_iso_8601(expiration, false);
      if (exp) {
        real_clock::time_point now = real_clock::now();
        if (now >= *exp) {
          ldpp_dout(dpp, 0) << "ERROR: Token expired" << dendl;
          return result_t::reject(-EPERM);
        }
      } else {
        ldpp_dout(dpp, 0) << "ERROR: Invalid expiration: " << expiration << dendl;
        return result_t::reject(-EPERM);
      }
    }
  }
  //Check for signature mismatch
  const VersionAbstractor::server_signature_t server_signature = \
    signature_factory(cct, token.secret_access_key, string_to_sign);
  auto compare = signature.compare(server_signature);

  ldpp_dout(dpp, 15) << "string_to_sign="
                 << rgw::crypt_sanitize::log_content{string_to_sign}
                 << dendl;
  ldpp_dout(dpp, 15) << "server signature=" << server_signature << dendl;
  ldpp_dout(dpp, 15) << "client signature=" << signature << dendl;
  ldpp_dout(dpp, 15) << "compare=" << compare << dendl;

  if (compare != 0) {
    return result_t::reject(-ERR_SIGNATURE_NO_MATCH);
  }

  // Get all the authorization info
  RGWUserInfo user_info;
  rgw_user user_id;
  vector<string> role_policies;
  string role_name;
  if (! token.roleId.empty()) {
    RGWRole role(s->cct, store, token.roleId);
    if (role.get_by_id() < 0) {
      return result_t::deny(-EPERM);
    }
    vector<string> role_policy_names = role.get_role_policy_names();
    for (auto& policy_name : role_policy_names) {
      string perm_policy;
      if (int ret = role.get_role_policy(policy_name, perm_policy); ret == 0) {
        role_policies.push_back(std::move(perm_policy));
      }
    }
    if (! token.policy.empty()) {
      role_policies.push_back(std::move(token.policy));
    }
    // This is mostly needed to assign the owner of a bucket during its creation
    user_id = token.user;
    role_name = role.get_name();
  }

  if (! token.user.empty() && token.acct_type != TYPE_ROLE) {
    // get user info
    int ret = rgw_get_user_info_by_uid(store, token.user, user_info, NULL);
    if (ret < 0) {
      ldpp_dout(dpp, 5) << "ERROR: failed reading user info: uid=" << token.user << dendl;
      return result_t::reject(-EPERM);
    }
  }

  if (token.acct_type == TYPE_KEYSTONE || token.acct_type == TYPE_LDAP) {
    auto apl = remote_apl_factory->create_apl_remote(cct, s, get_acl_strategy(),
                                            get_creds_info(token));
    return result_t::grant(std::move(apl), completer_factory(boost::none));
  } else if (token.acct_type == TYPE_ROLE) {
    auto apl = role_apl_factory->create_apl_role(cct, s, role_name, user_id, role_policies);
    return result_t::grant(std::move(apl), completer_factory(token.secret_access_key));
  } else { // This is for all local users of type TYPE_RGW or TYPE_NONE
    string subuser;
    auto apl = local_apl_factory->create_apl_local(cct, s, user_info, subuser, token.perm_mask);
    return result_t::grant(std::move(apl), completer_factory(token.secret_access_key));
  }
}
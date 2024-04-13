AWSGeneralAbstractor::get_auth_data_v2(const req_state* const s) const
{
  boost::string_view access_key_id;
  boost::string_view signature;
  boost::string_view session_token;
  bool qsr = false;

  const char* http_auth = s->info.env->get("HTTP_AUTHORIZATION");
  if (! http_auth || http_auth[0] == '\0') {
    /* Credentials are provided in query string. We also need to verify
     * the "Expires" parameter now. */
    access_key_id = s->info.args.get("AWSAccessKeyId");
    signature = s->info.args.get("Signature");
    qsr = true;

    boost::string_view expires = s->info.args.get("Expires");
    if (expires.empty()) {
      throw -EPERM;
    }

    /* It looks we have the guarantee that expires is a null-terminated,
     * and thus string_view::data() can be safely used. */
    const time_t exp = atoll(expires.data());
    time_t now;
    time(&now);

    if (now >= exp) {
      throw -EPERM;
    }
    if (s->info.args.exists("X-Amz-Security-Token")) {
      session_token = s->info.args.get("X-Amz-Security-Token");
      if (session_token.size() == 0) {
        throw -EPERM;
      }
    }

  } else {
    /* The "Authorization" HTTP header is being used. */
    const boost::string_view auth_str(http_auth + strlen("AWS "));
    const size_t pos = auth_str.rfind(':');
    if (pos != boost::string_view::npos) {
      access_key_id = auth_str.substr(0, pos);
      signature = auth_str.substr(pos + 1);
    }

    if (s->info.env->exists("HTTP_X_AMZ_SECURITY_TOKEN")) {
      session_token = s->info.env->get("HTTP_X_AMZ_SECURITY_TOKEN");
      if (session_token.size() == 0) {
        throw -EPERM;
      }
    }
  }

  /* Let's canonize the HTTP headers that are covered by the AWS auth v2. */
  std::string string_to_sign;
  utime_t header_time;
  if (! rgw_create_s3_canonical_header(s->info, &header_time, string_to_sign,
        qsr)) {
    ldout(cct, 10) << "failed to create the canonized auth header\n"
                   << rgw::crypt_sanitize::auth{s,string_to_sign} << dendl;
    throw -EPERM;
  }

  ldout(cct, 10) << "string_to_sign:\n"
                 << rgw::crypt_sanitize::auth{s,string_to_sign} << dendl;

  if (!qsr && !is_time_skew_ok(header_time)) {
    throw -ERR_REQUEST_TIME_SKEWED;
  }

  return {
    std::move(access_key_id),
    std::move(signature),
    std::move(session_token),
    std::move(string_to_sign),
    rgw::auth::s3::get_v2_signature,
    null_completer_factory
  };
}
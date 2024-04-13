AWSGeneralAbstractor::get_auth_data_v4(const req_state* const s,
                                       const bool using_qs) const
{
  boost::string_view access_key_id;
  boost::string_view signed_hdrs;

  boost::string_view date;
  boost::string_view credential_scope;
  boost::string_view client_signature;
  boost::string_view session_token;

  int ret = rgw::auth::s3::parse_v4_credentials(s->info,
						access_key_id,
						credential_scope,
						signed_hdrs,
						client_signature,
						date,
						session_token,
						using_qs);
  if (ret < 0) {
    throw ret;
  }

  /* craft canonical headers */
  boost::optional<std::string> canonical_headers = \
    get_v4_canonical_headers(s->info, signed_hdrs, using_qs);
  if (canonical_headers) {
    using sanitize = rgw::crypt_sanitize::log_content;
    ldout(s->cct, 10) << "canonical headers format = "
                      << sanitize{*canonical_headers} << dendl;
  } else {
    throw -EPERM;
  }

  bool is_non_s3_op = false;
  if (s->op_type == RGW_STS_GET_SESSION_TOKEN ||
      s->op_type == RGW_STS_ASSUME_ROLE ||
      s->op_type == RGW_STS_ASSUME_ROLE_WEB_IDENTITY ||
      s->op_type == RGW_OP_CREATE_ROLE ||
      s->op_type == RGW_OP_DELETE_ROLE ||
      s->op_type == RGW_OP_GET_ROLE ||
      s->op_type == RGW_OP_MODIFY_ROLE ||
      s->op_type == RGW_OP_LIST_ROLES ||
      s->op_type == RGW_OP_PUT_ROLE_POLICY ||
      s->op_type == RGW_OP_GET_ROLE_POLICY ||
      s->op_type == RGW_OP_LIST_ROLE_POLICIES ||
      s->op_type == RGW_OP_DELETE_ROLE_POLICY ||
      s->op_type == RGW_OP_PUT_USER_POLICY ||
      s->op_type == RGW_OP_GET_USER_POLICY ||
      s->op_type == RGW_OP_LIST_USER_POLICIES ||
      s->op_type == RGW_OP_DELETE_USER_POLICY) {
    is_non_s3_op = true;
  }

  const char* exp_payload_hash = nullptr;
  string payload_hash;
  if (is_non_s3_op) {
    //For non s3 ops, we need to calculate the payload hash
    payload_hash = s->info.args.get("PayloadHash");
    exp_payload_hash = payload_hash.c_str();
  } else {
    /* Get the expected hash. */
    exp_payload_hash = rgw::auth::s3::get_v4_exp_payload_hash(s->info);
  }

  /* Craft canonical URI. Using std::move later so let it be non-const. */
  auto canonical_uri = rgw::auth::s3::get_v4_canonical_uri(s->info);

  /* Craft canonical query string. std::moving later so non-const here. */
  auto canonical_qs = rgw::auth::s3::get_v4_canonical_qs(s->info, using_qs);

  /* Craft canonical request. */
  auto canonical_req_hash = \
    rgw::auth::s3::get_v4_canon_req_hash(s->cct,
                                         s->info.method,
                                         std::move(canonical_uri),
                                         std::move(canonical_qs),
                                         std::move(*canonical_headers),
                                         signed_hdrs,
                                         exp_payload_hash);

  auto string_to_sign = \
    rgw::auth::s3::get_v4_string_to_sign(s->cct,
                                         AWS4_HMAC_SHA256_STR,
                                         date,
                                         credential_scope,
                                         std::move(canonical_req_hash));

  const auto sig_factory = std::bind(rgw::auth::s3::get_v4_signature,
                                     credential_scope,
                                     std::placeholders::_1,
                                     std::placeholders::_2,
                                     std::placeholders::_3);

  /* Requests authenticated with the Query Parameters are treated as unsigned.
   * From "Authenticating Requests: Using Query Parameters (AWS Signature
   * Version 4)":
   *
   *   You don't include a payload hash in the Canonical Request, because
   *   when you create a presigned URL, you don't know the payload content
   *   because the URL is used to upload an arbitrary payload. Instead, you
   *   use a constant string UNSIGNED-PAYLOAD.
   *
   * This means we have absolutely no business in spawning completer. Both
   * aws4_auth_needs_complete and aws4_auth_streaming_mode are set to false
   * by default. We don't need to change that. */
  if (is_v4_payload_unsigned(exp_payload_hash) || is_v4_payload_empty(s) || is_non_s3_op) {
    return {
      access_key_id,
      client_signature,
      session_token,
      std::move(string_to_sign),
      sig_factory,
      null_completer_factory
    };
  } else {
    /* We're going to handle a signed payload. Be aware that even empty HTTP
     * body (no payload) requires verification:
     *
     *   The x-amz-content-sha256 header is required for all AWS Signature
     *   Version 4 requests. It provides a hash of the request payload. If
     *   there is no payload, you must provide the hash of an empty string. */
    if (!is_v4_payload_streamed(exp_payload_hash)) {
      ldout(s->cct, 10) << "delaying v4 auth" << dendl;

      /* payload in a single chunk */
      switch (s->op_type)
      {
        case RGW_OP_CREATE_BUCKET:
        case RGW_OP_PUT_OBJ:
        case RGW_OP_PUT_ACLS:
        case RGW_OP_PUT_CORS:
        case RGW_OP_INIT_MULTIPART: // in case that Init Multipart uses CHUNK encoding
        case RGW_OP_COMPLETE_MULTIPART:
        case RGW_OP_SET_BUCKET_VERSIONING:
        case RGW_OP_DELETE_MULTI_OBJ:
        case RGW_OP_ADMIN_SET_METADATA:
        case RGW_OP_SET_BUCKET_WEBSITE:
        case RGW_OP_PUT_BUCKET_POLICY:
        case RGW_OP_PUT_OBJ_TAGGING:
        case RGW_OP_PUT_LC:
        case RGW_OP_SET_REQUEST_PAYMENT:
        case RGW_OP_PUBSUB_NOTIF_CREATE:
        case RGW_OP_PUT_BUCKET_OBJ_LOCK:
        case RGW_OP_PUT_OBJ_RETENTION:
        case RGW_OP_PUT_OBJ_LEGAL_HOLD:
        case RGW_STS_GET_SESSION_TOKEN:
        case RGW_STS_ASSUME_ROLE:
          break;
        default:
          dout(10) << "ERROR: AWS4 completion for this operation NOT IMPLEMENTED" << dendl;
          throw -ERR_NOT_IMPLEMENTED;
      }

      const auto cmpl_factory = std::bind(AWSv4ComplSingle::create,
                                          s,
                                          std::placeholders::_1);
      return {
        access_key_id,
        client_signature,
        session_token,
        std::move(string_to_sign),
        sig_factory,
        cmpl_factory
      };
    } else {
      /* IMHO "streamed" doesn't fit too good here. I would prefer to call
       * it "chunked" but let's be coherent with Amazon's terminology. */

      dout(10) << "body content detected in multiple chunks" << dendl;

      /* payload in multiple chunks */

      switch(s->op_type)
      {
        case RGW_OP_PUT_OBJ:
          break;
        default:
          dout(10) << "ERROR: AWS4 completion for this operation NOT IMPLEMENTED (streaming mode)" << dendl;
          throw -ERR_NOT_IMPLEMENTED;
      }

      dout(10) << "aws4 seed signature ok... delaying v4 auth" << dendl;

      /* In the case of streamed payload client sets the x-amz-content-sha256
       * to "STREAMING-AWS4-HMAC-SHA256-PAYLOAD" but uses "UNSIGNED-PAYLOAD"
       * when constructing the Canonical Request. */

      /* In the case of single-chunk upload client set the header's value is
       * coherent with the one used for Canonical Request crafting. */

      /* In the case of query string-based authentication there should be no
       * x-amz-content-sha256 header and the value "UNSIGNED-PAYLOAD" is used
       * for CanonReq. */
      const auto cmpl_factory = std::bind(AWSv4ComplMulti::create,
                                          s,
                                          date,
                                          credential_scope,
                                          client_signature,
                                          std::placeholders::_1);
      return {
        access_key_id,
        client_signature,
        session_token,
        std::move(string_to_sign),
        sig_factory,
        cmpl_factory
      };
    }
  }
}
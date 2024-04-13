int RGWPostObj_ObjStore_S3::get_policy()
{
  if (part_bl(parts, "policy", &s->auth.s3_postobj_creds.encoded_policy)) {
    bool aws4_auth = false;

    /* x-amz-algorithm handling */
    using rgw::auth::s3::AWS4_HMAC_SHA256_STR;
    if ((part_str(parts, "x-amz-algorithm", &s->auth.s3_postobj_creds.x_amz_algorithm)) &&
        (s->auth.s3_postobj_creds.x_amz_algorithm == AWS4_HMAC_SHA256_STR)) {
      ldout(s->cct, 0) << "Signature verification algorithm AWS v4 (AWS4-HMAC-SHA256)" << dendl;
      aws4_auth = true;
    } else {
      ldout(s->cct, 0) << "Signature verification algorithm AWS v2" << dendl;
    }

    // check that the signature matches the encoded policy
    if (aws4_auth) {
      /* AWS4 */

      /* x-amz-credential handling */
      if (!part_str(parts, "x-amz-credential",
                    &s->auth.s3_postobj_creds.x_amz_credential)) {
        ldout(s->cct, 0) << "No S3 aws4 credential found!" << dendl;
        err_msg = "Missing aws4 credential";
        return -EINVAL;
      }

      /* x-amz-signature handling */
      if (!part_str(parts, "x-amz-signature",
                    &s->auth.s3_postobj_creds.signature)) {
        ldout(s->cct, 0) << "No aws4 signature found!" << dendl;
        err_msg = "Missing aws4 signature";
        return -EINVAL;
      }

      /* x-amz-date handling */
      std::string received_date_str;
      if (!part_str(parts, "x-amz-date", &received_date_str)) {
        ldout(s->cct, 0) << "No aws4 date found!" << dendl;
        err_msg = "Missing aws4 date";
        return -EINVAL;
      }
    } else {
      /* AWS2 */

      // check that the signature matches the encoded policy
      if (!part_str(parts, "AWSAccessKeyId",
                    &s->auth.s3_postobj_creds.access_key)) {
        ldout(s->cct, 0) << "No S3 aws2 access key found!" << dendl;
        err_msg = "Missing aws2 access key";
        return -EINVAL;
      }

      if (!part_str(parts, "signature", &s->auth.s3_postobj_creds.signature)) {
        ldout(s->cct, 0) << "No aws2 signature found!" << dendl;
        err_msg = "Missing aws2 signature";
        return -EINVAL;
      }
    }

    part_str(parts, "x-amz-security-token", &s->auth.s3_postobj_creds.x_amz_security_token);

    /* FIXME: this is a makeshift solution. The browser upload authentication will be
     * handled by an instance of rgw::auth::Completer spawned in Handler's authorize()
     * method. */
    const int ret = rgw::auth::Strategy::apply(this, auth_registry_ptr->get_s3_post(), s);
    if (ret != 0) {
      return -EACCES;
    } else {
      /* Populate the owner info. */
      s->owner.set_id(s->user->user_id);
      s->owner.set_name(s->user->display_name);
      ldout(s->cct, 20) << "Successful Signature Verification!" << dendl;
    }

    ceph::bufferlist decoded_policy;
    try {
      decoded_policy.decode_base64(s->auth.s3_postobj_creds.encoded_policy);
    } catch (buffer::error& err) {
      ldout(s->cct, 0) << "failed to decode_base64 policy" << dendl;
      err_msg = "Could not decode policy";
      return -EINVAL;
    }

    decoded_policy.append('\0'); // NULL terminate
    ldout(s->cct, 20) << "POST policy: " << decoded_policy.c_str() << dendl;


    int r = post_policy.from_json(decoded_policy, err_msg);
    if (r < 0) {
      if (err_msg.empty()) {
	err_msg = "Failed to parse policy";
      }
      ldout(s->cct, 0) << "failed to parse policy" << dendl;
      return -EINVAL;
    }

    if (aws4_auth) {
      /* AWS4 */
      post_policy.set_var_checked("x-amz-signature");
    } else {
      /* AWS2 */
      post_policy.set_var_checked("AWSAccessKeyId");
      post_policy.set_var_checked("signature");
    }
    post_policy.set_var_checked("policy");

    r = post_policy.check(&env, err_msg);
    if (r < 0) {
      if (err_msg.empty()) {
	err_msg = "Policy check failed";
      }
      ldout(s->cct, 0) << "policy check failed" << dendl;
      return r;
    }

  } else {
    ldout(s->cct, 0) << "No attached policy found!" << dendl;
  }

  string canned_acl;
  part_str(parts, "acl", &canned_acl);

  RGWAccessControlPolicy_S3 s3policy(s->cct);
  ldout(s->cct, 20) << "canned_acl=" << canned_acl << dendl;
  if (s3policy.create_canned(s->owner, s->bucket_owner, canned_acl) < 0) {
    err_msg = "Bad canned ACLs";
    return -EINVAL;
  }

  policy = s3policy;

  return 0;
}
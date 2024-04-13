rgw::auth::s3::STSEngine::get_session_token(const boost::string_view& session_token,
                                            STS::SessionToken& token) const
{
  string decodedSessionToken;
  try {
    decodedSessionToken = rgw::from_base64(session_token);
  } catch (...) {
    ldout(cct, 0) << "ERROR: Invalid session token, not base64 encoded." << dendl;
    return -EINVAL;
  }

  auto* cryptohandler = cct->get_crypto_handler(CEPH_CRYPTO_AES);
  if (! cryptohandler) {
    return -EINVAL;
  }
  string secret_s = cct->_conf->rgw_sts_key;
  buffer::ptr secret(secret_s.c_str(), secret_s.length());
  int ret = 0;
  if (ret = cryptohandler->validate_secret(secret); ret < 0) {
    ldout(cct, 0) << "ERROR: Invalid secret key" << dendl;
    return -EINVAL;
  }
  string error;
  auto* keyhandler = cryptohandler->get_key_handler(secret, error);
  if (! keyhandler) {
    return -EINVAL;
  }
  error.clear();

  string decrypted_str;
  buffer::list en_input, dec_output;
  en_input = buffer::list::static_from_string(decodedSessionToken);

  ret = keyhandler->decrypt(en_input, dec_output, &error);
  if (ret < 0) {
    ldout(cct, 0) << "ERROR: Decryption failed: " << error << dendl;
    return -EPERM;
  } else {
    try {
      dec_output.append('\0');
      auto iter = dec_output.cbegin();
      decode(token, iter);
    } catch (const buffer::error& e) {
      ldout(cct, 0) << "ERROR: decode SessionToken failed: " << error << dendl;
      return -EINVAL;
    }
  }
  return 0;
}
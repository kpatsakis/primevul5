void ProtocolV1::reset_security()
{
  ldout(cct, 5) << __func__ << dendl;

  auth_meta.reset(new AuthConnectionMeta);
  authorizer_more.clear();
  session_security.reset();
}
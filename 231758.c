int RGWDeleteObj_ObjStore_SWIFT::verify_permission()
{
  op_ret = RGWDeleteObj_ObjStore::verify_permission();

  /* We have to differentiate error codes depending on whether user is
   * anonymous (401 Unauthorized) or he doesn't have necessary permissions
   * (403 Forbidden). */
  if (s->auth.identity->is_anonymous() && op_ret == -EACCES) {
    return -EPERM;
  } else {
    return op_ret;
  }
}
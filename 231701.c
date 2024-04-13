void RGWStatAccount_ObjStore_SWIFT::execute()
{
  RGWStatAccount_ObjStore::execute();
  op_ret = rgw_get_user_attrs_by_uid(store, s->user->user_id, attrs);
}
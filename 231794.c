int RGWCopyObj_ObjStore_SWIFT::init_dest_policy()
{
  dest_policy.create_default(s->user->user_id, s->user->display_name);

  return 0;
}
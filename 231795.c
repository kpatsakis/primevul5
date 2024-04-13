RGWOp *RGWHandler_REST_Obj_SWIFT::get_obj_op(bool get_data)
{
  if (is_acl_op()) {
    return new RGWGetACLs_ObjStore_SWIFT;
  }

  RGWGetObj_ObjStore_SWIFT *get_obj_op = new RGWGetObj_ObjStore_SWIFT;
  get_obj_op->set_get_data(get_data);
  return get_obj_op;
}
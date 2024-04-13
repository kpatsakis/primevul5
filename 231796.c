RGWOp *RGWHandler_REST_Obj_SWIFT::op_copy()
{
  return new RGWCopyObj_ObjStore_SWIFT;
}
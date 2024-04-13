RGWOp *RGWHandler_REST_Obj_SWIFT::op_delete()
{
  return new RGWDeleteObj_ObjStore_SWIFT;
}
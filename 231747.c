RGWOp *RGWHandler_REST_Obj_SWIFT::op_options()
{
  return new RGWOptionsCORS_ObjStore_SWIFT;
}
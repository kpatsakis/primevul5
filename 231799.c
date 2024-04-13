RGWOp *RGWHandler_REST_Service_SWIFT::op_head()
{
  return new RGWStatAccount_ObjStore_SWIFT;
}
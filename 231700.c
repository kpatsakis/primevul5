RGWOp *RGWHandler_REST_Service_SWIFT::op_get()
{
  return new RGWListBuckets_ObjStore_SWIFT;
}
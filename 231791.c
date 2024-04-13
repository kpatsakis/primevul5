RGWOp *RGWHandler_REST_Bucket_SWIFT::op_options()
{
  return new RGWOptionsCORS_ObjStore_SWIFT;
}
RGWOp *RGWHandler_REST_Bucket_SWIFT::op_delete()
{
  return new RGWDeleteBucket_ObjStore_SWIFT;
}
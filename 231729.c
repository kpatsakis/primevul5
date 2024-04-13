RGWOp *RGWHandler_REST_Bucket_SWIFT::op_put()
{
  if (is_acl_op()) {
    return new RGWPutACLs_ObjStore_SWIFT;
  }
  if(s->info.args.exists("extract-archive")) {
    return new RGWBulkUploadOp_ObjStore_SWIFT;
  }
  return new RGWCreateBucket_ObjStore_SWIFT;
}
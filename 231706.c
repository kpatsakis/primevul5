RGWOp *RGWHandler_REST_Obj_SWIFT::op_put()
{
  if (is_acl_op()) {
    return new RGWPutACLs_ObjStore_SWIFT;
  }
  if(s->info.args.exists("extract-archive")) {
    return new RGWBulkUploadOp_ObjStore_SWIFT;
  }
  if (s->init_state.src_bucket.empty())
    return new RGWPutObj_ObjStore_SWIFT;
  else
    return new RGWCopyObj_ObjStore_SWIFT;
}
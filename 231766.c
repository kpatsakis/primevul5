RGWOp *RGWHandler_REST_Service_SWIFT::op_put()
{
  if (s->info.args.exists("extract-archive")) {
    return new RGWBulkUploadOp_ObjStore_SWIFT;
  }
  return nullptr;
}
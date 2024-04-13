RGWOp *RGWHandler_REST_Service_SWIFT::op_post()
{
  if (s->info.args.exists("bulk-delete")) {
    return new RGWBulkDelete_ObjStore_SWIFT;
  }
  return new RGWPutMetadataAccount_ObjStore_SWIFT;
}
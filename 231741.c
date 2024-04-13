RGWOp *RGWHandler_REST_Service_SWIFT::op_delete()
{
  if (s->info.args.exists("bulk-delete")) {
    return new RGWBulkDelete_ObjStore_SWIFT;
  }
  return NULL;
}
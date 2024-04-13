RGWOp *RGWHandler_REST_Bucket_SWIFT::op_post()
{
  if (RGWFormPost::is_formpost_req(s)) {
    return new RGWFormPost;
  } else {
    return new RGWPutMetadataBucket_ObjStore_SWIFT;
  }
}
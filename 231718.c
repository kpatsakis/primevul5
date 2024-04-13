RGWOp *RGWHandler_REST_Obj_SWIFT::op_post()
{
  if (RGWFormPost::is_formpost_req(s)) {
    return new RGWFormPost;
  } else {
    return new RGWPutMetadataObject_ObjStore_SWIFT;
  }
}
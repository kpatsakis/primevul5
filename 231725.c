RGWOp *RGWHandler_REST_Bucket_SWIFT::get_obj_op(bool get_data)
{
  if (is_acl_op()) {
    return new RGWGetACLs_ObjStore_SWIFT;
  }

  if (get_data)
    return new RGWListBucket_ObjStore_SWIFT;
  else
    return new RGWStatBucket_ObjStore_SWIFT;
}
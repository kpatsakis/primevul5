RGWOp *RGWHandler_REST_Bucket_S3::get_obj_op(bool get_data)
{
  // Non-website mode
  if (get_data) {   
    int list_type = 1;
    s->info.args.get_int("list-type", &list_type, 1);
    switch (list_type) {
      case 1:
        return new RGWListBucket_ObjStore_S3;
      case 2:
        return new RGWListBucket_ObjStore_S3v2;
      default:
        ldpp_dout(s, 5) << __func__ << ": unsupported list-type " << list_type << dendl;
        return new RGWListBucket_ObjStore_S3;
    }
  } else {
    return new RGWStatBucket_ObjStore_S3;
  }
}
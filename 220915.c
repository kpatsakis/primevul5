int RGWCopyObj_ObjStore_S3::check_storage_class(const rgw_placement_rule& src_placement)
{
  if (src_placement == s->dest_placement) {
    /* can only copy object into itself if replacing attrs */
    s->err.message = "This copy request is illegal because it is trying to copy "
      "an object to itself without changing the object's metadata, "
      "storage class, website redirect location or encryption attributes.";
    ldout(s->cct, 0) << s->err.message << dendl;
    return -ERR_INVALID_REQUEST;
  }
  return 0;
}
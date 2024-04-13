void RGWGetLC_ObjStore_S3::execute()
{
  config.set_ctx(s->cct);

  map<string, bufferlist>::iterator aiter = s->bucket_attrs.find(RGW_ATTR_LC);
  if (aiter == s->bucket_attrs.end()) {
    op_ret = -ENOENT;
    return;
  }

  bufferlist::const_iterator iter{&aiter->second};
  try {
      config.decode(iter);
    } catch (const buffer::error& e) {
      ldout(s->cct, 0) << __func__ <<  "decode life cycle config failed" << dendl;
      op_ret = -EIO;
      return;
    }
}
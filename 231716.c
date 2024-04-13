RGWOp* RGWSwiftWebsiteHandler::get_ws_index_op()
{
  /* Retarget to get obj on requested index file. */
  if (! s->object.empty()) {
    s->object = s->object.name +
                s->bucket_info.website_conf.get_index_doc();
  } else {
    s->object = s->bucket_info.website_conf.get_index_doc();
  }

  auto getop = new RGWGetObj_ObjStore_SWIFT;
  getop->set_get_data(boost::algorithm::equals("GET", s->info.method));

  return getop;
}
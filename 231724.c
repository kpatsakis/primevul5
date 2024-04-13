void RGWFormPost::init(RGWRados* const store,
                       req_state* const s,
                       RGWHandler* const dialect_handler)
{
  prefix = std::move(s->object.name);
  s->object = rgw_obj_key();

  return RGWPostObj_ObjStore::init(store, s, dialect_handler);
}
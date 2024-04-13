  Item_cache_str(THD *thd, const Item *item):
    Item_cache(thd, item->field_type()), value(0),
    is_varbinary(item->type() == FIELD_ITEM &&
                 Item_cache_str::field_type() == MYSQL_TYPE_VARCHAR &&
                 !((const Item_field *) item)->field->has_charset())
  {
    collation.set(const_cast<DTCollation&>(item->collation));
  }
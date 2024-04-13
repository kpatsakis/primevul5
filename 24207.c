set_new_item_local_context(THD *thd, Item_ident *item, TABLE_LIST *table_ref)
{
  Name_resolution_context *context;
  if (!(context= new (thd->mem_root) Name_resolution_context))
    return TRUE;
  context->init();
  context->first_name_resolution_table=
    context->last_name_resolution_table= table_ref;
  item->context= context;
  return FALSE;
}
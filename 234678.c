static TABLE_LIST* embedding_sjm(Item *item)
{
  Item_field *item_field= (Item_field *) (item->real_item());
  TABLE_LIST *nest= item_field->field->table->pos_in_table_list->embedding;
  if (nest && nest->sj_mat_info && nest->sj_mat_info->is_used)
    return nest;
  else
    return NULL;
}
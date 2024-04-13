bool Item_func_like::find_selective_predicates_list_processor(void *arg)
{
  find_selective_predicates_list_processor_data *data=
    (find_selective_predicates_list_processor_data *) arg;
  if (use_sampling && used_tables() == data->table->map)
  {
    THD *thd= data->table->in_use;
    COND_STATISTIC *stat;
    Item *arg0;
    if (!(stat= (COND_STATISTIC *) thd->alloc(sizeof(COND_STATISTIC))))
      return TRUE;
    stat->cond= this;
    arg0= args[0]->real_item();
    if (args[1]->const_item() && arg0->type() == FIELD_ITEM)
      stat->field_arg= ((Item_field *)arg0)->field;
    else
      stat->field_arg= NULL;
    data->list.push_back(stat, thd->mem_root);
  }
  return FALSE;
}
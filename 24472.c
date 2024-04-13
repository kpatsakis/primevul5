void st_select_lex::collect_grouping_fields(THD *thd) 
{
  grouping_tmp_fields.empty();
  List_iterator<Item> li(join->fields_list);
  Item *item= li++;
  for (uint i= 0; i < master_unit()->derived->table->s->fields; i++, (item=li++))
  {
    for (ORDER *ord= join->group_list; ord; ord= ord->next)
    {
      if ((*ord->item)->eq((Item*)item, 0))
      {
	Grouping_tmp_field *grouping_tmp_field= 
	  new Grouping_tmp_field(master_unit()->derived->table->field[i], item);
	grouping_tmp_fields.push_back(grouping_tmp_field);
      }
    }
  }
}
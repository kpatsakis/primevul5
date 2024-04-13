static void calc_group_buffer(JOIN *join, ORDER *group)
{
  if (group)
    join->group= 1;
  calc_group_buffer(&join->tmp_table_param, group);
}
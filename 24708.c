SEL_TREE *Item_func_like::get_mm_tree(RANGE_OPT_PARAM *param, Item **cond_ptr)
{
  MEM_ROOT *tmp_root= param->mem_root;
  param->thd->mem_root= param->old_root;
  bool sargable_pattern= with_sargable_pattern();
  param->thd->mem_root= tmp_root;
  return sargable_pattern ?
    Item_bool_func2::get_mm_tree(param, cond_ptr) :
    Item_func::get_mm_tree(param, cond_ptr);
}
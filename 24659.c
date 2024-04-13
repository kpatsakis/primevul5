bool Item_in_optimizer::is_top_level_item()
{
  if (!invisible_mode())
    return ((Item_in_subselect *)args[1])->is_top_level_item();
  return false;
}
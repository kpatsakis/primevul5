bool Item_in_optimizer::invisible_mode()
{
  /* MAX/MIN transformed or EXISTS->IN prepared => do nothing */
 return (args[1]->type() != Item::SUBSELECT_ITEM ||
         ((Item_subselect *)args[1])->substype() ==
         Item_subselect::EXISTS_SUBS);
}
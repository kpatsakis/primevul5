void in_double::set(uint pos,Item *item)
{
  ((double*) base)[pos]= item->val_real();
}
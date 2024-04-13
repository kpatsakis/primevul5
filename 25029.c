void in_longlong::set(uint pos,Item *item)
{
  struct packed_longlong *buff= &((packed_longlong*) base)[pos];
  
  buff->val= item->val_int();
  buff->unsigned_flag= item->unsigned_flag;
}
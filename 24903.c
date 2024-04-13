void in_time::set(uint pos,Item *item)
{
  struct packed_longlong *buff= &((packed_longlong*) base)[pos];

  buff->val= item->val_time_packed(current_thd);
  buff->unsigned_flag= 1L;
}
bool Item_in_optimizer::is_expensive()
{
  DBUG_ASSERT(fixed);
  return args[0]->is_expensive() || args[1]->is_expensive();
}
bool Item_in_optimizer::is_expensive_processor(void *arg)
{
  DBUG_ASSERT(fixed);
  return args[0]->is_expensive_processor(arg) ||
         args[1]->is_expensive_processor(arg);
}
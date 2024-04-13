bool Item_in_optimizer::is_null()
{
  val_int();
  return null_value;
}
uchar *in_string::get_value(Item *item)
{
  return (uchar*) item->val_str(&tmp);
}
in_row::~in_row()
{
  if (base)
    delete [] (cmp_item_row*) base;
}
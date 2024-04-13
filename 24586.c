void Item_equal::sort(Item_field_cmpfunc compare, void *arg)
{
  bubble_sort<Item>(&equal_items, compare, arg);
}
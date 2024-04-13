void switch_to_nullable_trigger_fields(List<Item> &items, TABLE *table)
{
  Field** field= table->field_to_fill();

 /* True if we have NOT NULL fields and BEFORE triggers */
  if (field != table->field)
  {
    List_iterator_fast<Item> it(items);
    Item *item;

    while ((item= it++))
      item->walk(&Item::switch_to_nullable_fields_processor, 1, field);
    table->triggers->reset_extra_null_bitmap();
  }
}
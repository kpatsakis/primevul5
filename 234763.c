is_local_field (Item *field)
{
  return field->real_item()->type() == Item::FIELD_ITEM
     && !(field->used_tables() & OUTER_REF_TABLE_BIT)
    && !((Item_field *)field->real_item())->get_depended_from();
}
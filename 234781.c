part_of_refkey(TABLE *table,Field *field)
{
  JOIN_TAB *join_tab= table->reginfo.join_tab;
  if (!join_tab)
    return (Item*) 0;             // field from outer non-select (UPDATE,...)

  uint ref_parts= join_tab->ref.key_parts;
  if (ref_parts) /* if it's ref/eq_ref/ref_or_null */
  {
    uint key= join_tab->ref.key;
    KEY *key_info= join_tab->get_keyinfo_by_key_no(key);
    KEY_PART_INFO *key_part= key_info->key_part;

    for (uint part=0 ; part < ref_parts ; part++,key_part++)
    {
      if (field->eq(key_part->field))
      {
        /*
          Found the field in the key. Check that 
           1. ref_or_null doesn't alternate this component between a value and
              a NULL
           2. index fully covers the key
        */
        if (part != join_tab->ref.null_ref_part &&            // (1)
            !(key_part->key_part_flag & HA_PART_KEY_SEG))     // (2)
        {
          return join_tab->ref.items[part];
        }
        break;
      }
    }
  }
  return (Item*) 0;
}
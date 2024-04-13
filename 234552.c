get_store_key(THD *thd, KEYUSE *keyuse, table_map used_tables,
	      KEY_PART_INFO *key_part, uchar *key_buff, uint maybe_null)
{
  if (!((~used_tables) & keyuse->used_tables))		// if const item
  {
    return new store_key_const_item(thd,
				    key_part->field,
				    key_buff + maybe_null,
				    maybe_null ? key_buff : 0,
				    key_part->length,
				    keyuse->val);
  }
  else if (keyuse->val->type() == Item::FIELD_ITEM ||
           (keyuse->val->type() == Item::REF_ITEM &&
	    ((((Item_ref*)keyuse->val)->ref_type() == Item_ref::OUTER_REF &&
              (*(Item_ref**)((Item_ref*)keyuse->val)->ref)->ref_type() ==
              Item_ref::DIRECT_REF) || 
             ((Item_ref*)keyuse->val)->ref_type() == Item_ref::VIEW_REF) &&
            keyuse->val->real_item()->type() == Item::FIELD_ITEM))
    return new store_key_field(thd,
			       key_part->field,
			       key_buff + maybe_null,
			       maybe_null ? key_buff : 0,
			       key_part->length,
			       ((Item_field*) keyuse->val->real_item())->field,
			       keyuse->val->real_item()->full_name());

  return new store_key_item(thd,
			    key_part->field,
			    key_buff + maybe_null,
			    maybe_null ? key_buff : 0,
			    key_part->length,
			    keyuse->val, FALSE);
}
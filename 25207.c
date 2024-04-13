Item* Item_equal::get_first(JOIN_TAB *context, Item *field_item)
{
  Item_equal_fields_iterator it(*this);
  Item *item;
  if (!field_item)
    return (it++);
  Field *field= ((Item_field *) (field_item->real_item()))->field;

  /*
    Of all equal fields, return the first one we can use. Normally, this is the
    field which belongs to the table that is the first in the join order.

    There is one exception to this: When semi-join materialization strategy is
    used, and the given field belongs to a table within the semi-join nest, we
    must pick the first field in the semi-join nest.

    Example: suppose we have a join order:

       ot1 ot2  SJ-Mat(it1  it2  it3)  ot3

    and equality ot2.col = it1.col = it2.col
    If we're looking for best substitute for 'it2.col', we should pick it1.col
    and not ot2.col.
    
    eliminate_item_equal() also has code that deals with equality substitution
    in presence of SJM nests.
  */

  TABLE_LIST *emb_nest;
  if (context != NO_PARTICULAR_TAB)
    emb_nest= context->emb_sj_nest;
  else
    emb_nest= field->table->pos_in_table_list->embedding;

  if (emb_nest && emb_nest->sj_mat_info && emb_nest->sj_mat_info->is_used)
  {
    /*
      It's a field from an materialized semi-join. We can substitute it for
       - a constant item 
       - a field from the same semi-join
       Find the first of such items:
    */
    while ((item= it++))
    {
      if (item->const_item() || 
          it.get_curr_field()->table->pos_in_table_list->embedding == emb_nest)
      {
        /*
          If we found given field then return NULL to avoid unnecessary
          substitution.
        */
        return (item != field_item) ? item : NULL;
      }
    }
  }
  else
  {
    /*
      The field is not in SJ-Materialization nest. We must return the first
      field in the join order. The field may be inside a semi-join nest, i.e 
      a join order may look like this:

          SJ-Mat(it1  it2)  ot1  ot2

      where we're looking what to substitute ot2.col for. In this case we must 
      still return it1.col, here's a proof why:

      First let's note that either it1.col or it2.col participates in 
      subquery's IN-equality. It can't be otherwise, because materialization is
      only applicable to uncorrelated subqueries, so the only way we could
      infer "it1.col=ot1.col" is from the IN-equality. Ok, so IN-eqality has 
      it1.col or it2.col on its inner side. it1.col is first such item in the
      join order, so it's not possible for SJ-Mat to be
      SJ-Materialization-lookup, it is SJ-Materialization-Scan. The scan part
      of this strategy will unpack value of it1.col=it2.col into it1.col
      (that's the first equal item inside the subquery), and we'll be able to
      get it from there. qed.
    */

    return equal_items.head();
  }
  // Shouldn't get here.
  DBUG_ASSERT(0);
  return NULL;
}
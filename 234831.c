static uint get_semi_join_select_list_index(Field *field)
{
  uint res= UINT_MAX;
  TABLE_LIST *emb_sj_nest;
  if ((emb_sj_nest= field->table->pos_in_table_list->embedding) &&
      emb_sj_nest->sj_on_expr)
  {
    Item_in_subselect *subq_pred= emb_sj_nest->sj_subq_pred;
    st_select_lex *subq_lex= subq_pred->unit->first_select();
    if (subq_pred->left_expr->cols() == 1)
    {
      Item *sel_item= subq_lex->ref_pointer_array[0];
      if (sel_item->type() == Item::FIELD_ITEM &&
          ((Item_field*)sel_item)->field->eq(field))
      {
        res= 0;
      }
    }
    else
    {
      for (uint i= 0; i < subq_pred->left_expr->cols(); i++)
      {
        Item *sel_item= subq_lex->ref_pointer_array[i];
        if (sel_item->type() == Item::FIELD_ITEM &&
            ((Item_field*)sel_item)->field->eq(field))
        {
          res= i;
          break;
        }
      }
    }
  }
  return res;
}
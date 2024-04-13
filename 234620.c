can_change_cond_ref_to_const(Item_bool_func2 *target,
                             Item *target_expr, Item *target_value,
                             Item_bool_func2 *source,
                             Item *source_expr, Item *source_const)
{
  return target_expr->eq(source_expr,0) &&
         target_value != source_const &&
         target->compare_type_handler()->
           can_change_cond_ref_to_const(target, target_expr, target_value,
                                        source, source_expr, source_const);
}
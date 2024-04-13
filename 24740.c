Item* Item_func_case_simple::propagate_equal_fields(THD *thd,
                                                    const Context &ctx,
                                                    COND_EQUAL *cond)
{
  const Type_handler *first_expr_cmp_handler;

  first_expr_cmp_handler= args[0]->type_handler_for_comparison();
  /*
    Cannot replace the CASE (the switch) argument if
    there are multiple comparison types were found, or found a single
    comparison type that is not equal to args[0]->cmp_type().

    - Example: multiple comparison types, can't propagate:
        WHERE CASE str_column
              WHEN 'string' THEN TRUE
              WHEN 1 THEN TRUE
              ELSE FALSE END;

    - Example: a single incompatible comparison type, can't propagate:
        WHERE CASE str_column
              WHEN DATE'2001-01-01' THEN TRUE
              ELSE FALSE END;

    - Example: a single incompatible comparison type, can't propagate:
        WHERE CASE str_column
              WHEN 1 THEN TRUE
              ELSE FALSE END;

    - Example: a single compatible comparison type, ok to propagate:
        WHERE CASE str_column
              WHEN 'str1' THEN TRUE
              WHEN 'str2' THEN TRUE
              ELSE FALSE END;
  */
  if (m_found_types == (1UL << first_expr_cmp_handler->cmp_type()))
    propagate_and_change_item_tree(thd, &args[0], cond,
      Context(ANY_SUBST, first_expr_cmp_handler, cmp_collation.collation));

  /*
    These arguments are in comparison.
    Allow invariants of the same value during propagation.
    Note, as we pass ANY_SUBST, none of the WHEN arguments will be
    replaced to zero-filled constants (only IDENTITY_SUBST allows this).
    Such a change for WHEN arguments would require rebuilding cmp_items.
  */
  uint i, count= when_count();
  for (i= 1; i <= count; i++)
  {
    Type_handler_hybrid_field_type tmp(first_expr_cmp_handler);
    if (!tmp.aggregate_for_comparison(args[i]->type_handler_for_comparison()))
      propagate_and_change_item_tree(thd, &args[i], cond,
        Context(ANY_SUBST, tmp.type_handler(), cmp_collation.collation));
  }

  // THEN and ELSE arguments (they are not in comparison)
  for (; i < arg_count; i++)
    propagate_and_change_item_tree(thd, &args[i], cond, Context_identity());

  return this;
}
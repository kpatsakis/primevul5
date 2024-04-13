int st_select_lex_unit::save_union_explain(Explain_query *output)
{
  SELECT_LEX *first= first_select();

  if (output->get_union(first->select_number))
    return 0; /* Already added */
    
  Explain_union *eu= 
    new (output->mem_root) Explain_union(output->mem_root, 
                                         thd->lex->analyze_stmt);

  if (with_element && with_element->is_recursive)
    eu->is_recursive_cte= true;
 
  if (derived)
    eu->connection_type= Explain_node::EXPLAIN_NODE_DERIVED;
  /* 
    Note: Non-merged semi-joins cannot be made out of UNIONs currently, so we
    dont ever set EXPLAIN_NODE_NON_MERGED_SJ.
  */

  for (SELECT_LEX *sl= first; sl; sl= sl->next_select())
    eu->add_select(sl->select_number);

  eu->fake_select_type= "UNION RESULT";
  eu->using_filesort= MY_TEST(global_parameters()->order_list.first);
  eu->using_tmp= union_needs_tmp_table();

  // Save the UNION node
  output->add_node(eu);

  if (eu->get_select_id() == 1)
    output->query_plan_ready();

  return 0;
}
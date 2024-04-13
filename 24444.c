int LEX::print_explain(select_result_sink *output, uint8 explain_flags,
                       bool is_analyze, bool *printed_anything)
{
  int res;
  if (explain && explain->have_query_plan())
  {
    res= explain->print_explain(output, explain_flags, is_analyze);
    *printed_anything= true;
  }
  else
  {
    res= 0;
    *printed_anything= false;
  }
  return res;
}
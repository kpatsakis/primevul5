delete_all_contexts (vcxt)
     VAR_CONTEXT *vcxt;
{
  VAR_CONTEXT *v, *t;

  for (v = vcxt; v != global_variables; v = t)
    {
      t = v->down;
      dispose_var_context (v);
    }    

  delete_all_variables (global_variables->table);
  shell_variables = global_variables;
}
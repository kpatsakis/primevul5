shell_control_structure (type)
     enum command_type type;
{
  switch (type)
    {
#if defined (ARITH_FOR_COMMAND)
    case cm_arith_for:
#endif
#if defined (SELECT_COMMAND)
    case cm_select:
#endif
#if defined (DPAREN_ARITHMETIC)
    case cm_arith:
#endif
#if defined (COND_COMMAND)
    case cm_cond:
#endif
    case cm_case:
    case cm_while:
    case cm_until:
    case cm_if:
    case cm_for:
    case cm_group:
    case cm_function_def:
      return (1);

    default:
      return (0);
    }
}
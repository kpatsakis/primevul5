builtin_status (result)
     int result;
{
  int r;

  switch (result)
    {
    case EX_USAGE:
      r = EX_BADUSAGE;
      break;
    case EX_REDIRFAIL:
    case EX_BADSYNTAX:
    case EX_BADASSIGN:
    case EX_EXPFAIL:
      r = EXECUTION_FAILURE;
      break;
    default:
      r = EXECUTION_SUCCESS;
      break;
    }
  return (r);
}
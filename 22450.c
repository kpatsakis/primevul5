task_completed_or_cancelled (GTask *task)
{
  if (g_task_get_completed (task))
    return TRUE;
  else if (g_task_return_error_if_cancelled (task))
      return TRUE;
  else
    return FALSE;
}
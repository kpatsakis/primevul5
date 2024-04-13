bool TABLE_REF::is_access_triggered()
{
  for (uint i = 0; i < key_parts; i++)
  {
    if (cond_guards[i])
      return TRUE;
  }
  return FALSE;
}
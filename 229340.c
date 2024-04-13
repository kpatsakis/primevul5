mysql_free_result(MYSQL_RES *result)
{
  if (result)
  {
    if (result->handle && result->handle->status == MYSQL_STATUS_USE_RESULT)
    {
      result->handle->methods->db_skip_result(result->handle);
      result->handle->status=MYSQL_STATUS_READY;
    }
    free_rows(result->data);
    if (result->fields)
      ma_free_root(&result->field_alloc,MYF(0));
    if (result->row)
      free(result->row);
    free(result);
  }
  return;
}
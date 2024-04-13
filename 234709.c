void explain_append_mrr_info(QUICK_RANGE_SELECT *quick, String *res)
{
  char mrr_str_buf[128];
  mrr_str_buf[0]=0;
  int len;
  handler *h= quick->head->file;
  len= h->multi_range_read_explain_info(quick->mrr_flags, mrr_str_buf,
                                        sizeof(mrr_str_buf));
  if (len > 0)
  {
    //res->append(STRING_WITH_LEN("; "));
    res->append(mrr_str_buf, len);
  }
}
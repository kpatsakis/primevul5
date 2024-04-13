test_if_subpart(ORDER *a,ORDER *b)
{
  for (; a && b; a=a->next,b=b->next)
  {
    if ((*a->item)->eq(*b->item,1))
      a->direction=b->direction;
    else
      return 0;
  }
  return MY_TEST(!b);
}
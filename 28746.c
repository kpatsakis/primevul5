freefileinfo (struct fileinfo *f)
{
  while (f)
    {
      struct fileinfo *next = f->next;
      xfree (f->name);
      if (f->linkto)
        xfree (f->linkto);
      xfree (f);
      f = next;
    }
}

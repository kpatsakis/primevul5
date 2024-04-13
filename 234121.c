add_link_defer (struct cpio_file_stat *file_hdr)
{
  struct deferment *d;
  d = create_deferment (file_hdr);
  d->next = deferouts;
  deferouts = d;
}
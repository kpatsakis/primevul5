dispose_fd_bitmap (fdbp)
     struct fd_bitmap *fdbp;
{
  FREE (fdbp->bitmap);
  free (fdbp);
}
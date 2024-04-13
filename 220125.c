void LibRaw::ljpeg_end(struct jhead *jh)
{
  int c;
  FORC4 if (jh->free[c]) free(jh->free[c]);
  free(jh->row);
}
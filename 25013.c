bool in_vector::find(Item *item)
{
  uchar *result=get_value(item);
  if (!result || !used_count)
    return false;				// Null value

  uint start,end;
  start=0; end=used_count-1;
  while (start != end)
  {
    uint mid=(start+end+1)/2;
    int res;
    if ((res=(*compare)(collation, base+mid*size, result)) == 0)
      return true;
    if (res < 0)
      start=mid;
    else
      end=mid-1;
  }
  return ((*compare)(collation, base+start*size, result) == 0);
}
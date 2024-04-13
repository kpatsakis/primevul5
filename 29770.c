int FoFiType1C::getDeltaIntArray(int *arr, int maxLen) {
  int x;
  int n, i;

  if ((n = nOps) > maxLen) {
    n = maxLen;
  }
  x = 0;
  for (i = 0; i < n; ++i) {
    x += (int)ops[i].num;
    arr[i] = x;
  }
  return n;
}

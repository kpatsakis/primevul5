GfxICCBasedColorSpace *GfxICCBasedCache::lookup(int numA, int genA)
{
  int i;

  if (cache[0].num == numA && cache[0].gen == genA) {
    return (GfxICCBasedColorSpace *)cache[0].colorSpace->copy();
  }
  for (i = 1;i < GFX_ICCBASED_CACHE_SIZE && cache[i].num > 0;i++) {
    if (cache[i].num == numA && cache[i].gen == genA) {
      int j;
      GfxICCBasedCache hit = cache[i];

      for (j = i;j > 0;j--) {
	if (cache[j - 1].num > 0) cache[j] = cache[j-1];
      }
      cache[0] = hit;
      return (GfxICCBasedColorSpace *)hit.colorSpace->copy();
    }
  }
  return NULL;
}

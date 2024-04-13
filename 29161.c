void GfxICCBasedCache::put(int numA, int genA,
  GfxICCBasedColorSpace *cs)
{
  int i;

  if (cache[GFX_ICCBASED_CACHE_SIZE-1].num > 0) {
    delete cache[GFX_ICCBASED_CACHE_SIZE-1].colorSpace;
  }
  for (i = GFX_ICCBASED_CACHE_SIZE-1; i > 0; i--) {
    if (cache[i - 1].num > 0) cache[i] = cache[i - 1];
  }
  cache[0].num = numA;
  cache[0].gen = genA;
  cache[0].colorSpace = (GfxICCBasedColorSpace *)cs->copy();
}

T3FontCache::T3FontCache(Ref *fontIDA, double m11A, double m12A,
			 double m21A, double m22A,
			 int glyphXA, int glyphYA, int glyphWA, int glyphHA,
			 GBool validBBoxA, GBool aa) {
  int i;

  fontID = *fontIDA;
  m11 = m11A;
  m12 = m12A;
  m21 = m21A;
  m22 = m22A;
  glyphX = glyphXA;
  glyphY = glyphYA;
  glyphW = glyphWA;
  glyphH = glyphHA;
  validBBox = validBBoxA;
  if (aa) {
    glyphSize = glyphW * glyphH;
  } else {
    glyphSize = ((glyphW + 7) >> 3) * glyphH;
  }
  cacheAssoc = 8;
  if (glyphSize <= 256) {
    cacheSets = 8;
  } else if (glyphSize <= 512) {
    cacheSets = 4;
  } else if (glyphSize <= 1024) {
    cacheSets = 2;
  } else if (glyphSize <= 2048) {
    cacheSets = 1;
    cacheAssoc = 4;
  } else if (glyphSize <= 4096) {
    cacheSets = 1;
    cacheAssoc = 2;
  } else {
    cacheSets = 1;
    cacheAssoc = 1;
  }
  if (glyphSize < 10485760 / cacheAssoc / cacheSets) {
    cacheData = (Guchar *)gmallocn_checkoverflow(cacheSets * cacheAssoc, glyphSize);
  } else {
    error(-1, "Not creating cacheData for T3FontCache, it asked for too much memory.\n"
              "       This could teoretically result in wrong rendering,\n"
              "       but most probably the document is bogus.\n"
              "       Please report a bug if you think the rendering may be wrong because of this.");
    cacheData = NULL;
  }
  if (cacheData != NULL)
  {
    cacheTags = (T3FontCacheTag *)gmallocn(cacheSets * cacheAssoc,
					 sizeof(T3FontCacheTag));
    for (i = 0; i < cacheSets * cacheAssoc; ++i) {
      cacheTags[i].mru = i & (cacheAssoc - 1);
    }
  }
  else
  {
    cacheTags = NULL;
  }
}

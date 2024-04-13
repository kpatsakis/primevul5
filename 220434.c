TEST_IMPL(utf8_decode1) {
  const char* p;
  char b[32];
  int i;

  /* ASCII. */
  p = b;
  snprintf(b, sizeof(b), "%c\x7F", 0x00);
  ASSERT(0 == uv__utf8_decode1(&p, b + sizeof(b)));
  ASSERT(p == b + 1);
  ASSERT(127 == uv__utf8_decode1(&p, b + sizeof(b)));
  ASSERT(p == b + 2);

  /* Two-byte sequences. */
  p = b;
  snprintf(b, sizeof(b), "\xC2\x80\xDF\xBF");
  ASSERT(128 == uv__utf8_decode1(&p, b + sizeof(b)));
  ASSERT(p == b + 2);
  ASSERT(0x7FF == uv__utf8_decode1(&p, b + sizeof(b)));
  ASSERT(p == b + 4);

  /* Three-byte sequences. */
  p = b;
  snprintf(b, sizeof(b), "\xE0\xA0\x80\xEF\xBF\xBF");
  ASSERT(0x800 == uv__utf8_decode1(&p, b + sizeof(b)));
  ASSERT(p == b + 3);
  ASSERT(0xFFFF == uv__utf8_decode1(&p, b + sizeof(b)));
  ASSERT(p == b + 6);

  /* Four-byte sequences. */
  p = b;
  snprintf(b, sizeof(b), "\xF0\x90\x80\x80\xF4\x8F\xBF\xBF");
  ASSERT(0x10000 == uv__utf8_decode1(&p, b + sizeof(b)));
  ASSERT(p == b + 4);
  ASSERT(0x10FFFF == uv__utf8_decode1(&p, b + sizeof(b)));
  ASSERT(p == b + 8);

  /* Four-byte sequences > U+10FFFF; disallowed. */
  p = b;
  snprintf(b, sizeof(b), "\xF4\x90\xC0\xC0\xF7\xBF\xBF\xBF");
  ASSERT((unsigned) -1 == uv__utf8_decode1(&p, b + sizeof(b)));
  ASSERT(p == b + 4);
  ASSERT((unsigned) -1 == uv__utf8_decode1(&p, b + sizeof(b)));
  ASSERT(p == b + 8);

  /* Overlong; disallowed. */
  p = b;
  snprintf(b, sizeof(b), "\xC0\x80\xC1\x80");
  ASSERT((unsigned) -1 == uv__utf8_decode1(&p, b + sizeof(b)));
  ASSERT(p == b + 2);
  ASSERT((unsigned) -1 == uv__utf8_decode1(&p, b + sizeof(b)));
  ASSERT(p == b + 4);

  /* Surrogate pairs; disallowed. */
  p = b;
  snprintf(b, sizeof(b), "\xED\xA0\x80\xED\xA3\xBF");
  ASSERT((unsigned) -1 == uv__utf8_decode1(&p, b + sizeof(b)));
  ASSERT(p == b + 3);
  ASSERT((unsigned) -1 == uv__utf8_decode1(&p, b + sizeof(b)));
  ASSERT(p == b + 6);

  /* Simply illegal. */
  p = b;
  snprintf(b, sizeof(b), "\xF8\xF9\xFA\xFB\xFC\xFD\xFE\xFF");

  for (i = 1; i <= 8; i++) {
    ASSERT((unsigned) -1 == uv__utf8_decode1(&p, b + sizeof(b)));
    ASSERT(p == b + i);
  }

  return 0;
}
uint16_t Latin1::ConvertNonLatin1ToLatin1(uint16_t c) {
  DCHECK(c > Latin1::kMaxChar);
  switch (c) {
    // This are equivalent characters in unicode.
    case 0x39c:
    case 0x3bc:
      return 0xb5;
    // This is an uppercase of a Latin-1 character
    // outside of Latin-1.
    case 0x178:
      return 0xff;
  }
  return 0;
}
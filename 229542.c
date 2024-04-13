UnicodeStringTest::TestUInt16Pointers() {
    static const uint16_t carr[] = { 0x61, 0x62, 0x63, 0 };
    uint16_t arr[4];

    UnicodeString expected(u"abc");
    assertEquals("abc from pointer", expected, UnicodeString(carr));
    assertEquals("abc from pointer+length", expected, UnicodeString(carr, 3));
    assertEquals("abc from read-only-alias pointer", expected, UnicodeString(TRUE, carr, 3));

    UnicodeString alias(arr, 0, 4);
    alias.append(u'a').append(u'b').append(u'c');
    assertEquals("abc from writable alias", expected, alias);
    assertEquals("buffer=abc from writable alias", expected, UnicodeString(arr, 3));

    UErrorCode errorCode = U_ZERO_ERROR;
    int32_t length = UnicodeString(u"def").extract(arr, 4, errorCode);
    assertSuccess(WHERE, errorCode);
    assertEquals("def from extract()", UnicodeString(u"def"), UnicodeString(arr, length));
}
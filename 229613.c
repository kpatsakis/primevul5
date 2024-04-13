UnicodeStringTest::TestNullPointers() {
    assertTrue("empty from nullptr", UnicodeString(nullptr).isEmpty());
    assertTrue("empty from nullptr+length", UnicodeString(nullptr, 2).isEmpty());
    assertTrue("empty from read-only-alias nullptr", UnicodeString(TRUE, nullptr, 3).isEmpty());

    UnicodeString alias(nullptr, 4, 4);  // empty, no alias
    assertTrue("empty from writable alias", alias.isEmpty());
    alias.append(u'a').append(u'b').append(u'c');
    UnicodeString expected(u"abc");
    assertEquals("abc from writable alias", expected, alias);

    UErrorCode errorCode = U_ZERO_ERROR;
    UnicodeString(u"def").extract(nullptr, 0, errorCode);
    assertEquals("buffer overflow extracting to nullptr", U_BUFFER_OVERFLOW_ERROR, errorCode);
}
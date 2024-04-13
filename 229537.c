void UnicodeStringTest::TestUnescape(void) {
    UnicodeString IN("abc\\u4567 \\n\\r \\U00101234xyz\\x1\\x{5289}\\x1b", -1, US_INV);
    UnicodeString OUT("abc");
    OUT.append((UChar)0x4567);
    OUT.append(" ");
    OUT.append((UChar)0xA);
    OUT.append((UChar)0xD);
    OUT.append(" ");
    OUT.append((UChar32)0x00101234);
    OUT.append("xyz");
    OUT.append((UChar32)1).append((UChar32)0x5289).append((UChar)0x1b);
    UnicodeString result = IN.unescape();
    if (result != OUT) {
        errln("FAIL: " + prettify(IN) + ".unescape() -> " +
              prettify(result) + ", expected " +
              prettify(OUT));
    }

    // test that an empty string is returned in case of an error
    if (!UNICODE_STRING("wrong \\u sequence", 17).unescape().isEmpty()) {
        errln("FAIL: unescaping of a string with an illegal escape sequence did not return an empty string");
    }
}
UnicodeStringTest::TestUTF32() {
    // Input string length US_STACKBUF_SIZE to cause overflow of the
    // initially chosen fStackBuffer due to supplementary characters.
    static const UChar32 utf32[] = {
        0x41, 0xd900, 0x61, 0xdc00, -1, 0x110000, 0x5a, 0x50000, 0x7a,
        0x10000, 0x20000, 0xe0000, 0x10ffff
    };
    static const UChar expected_utf16[] = {
        0x41, 0xfffd, 0x61, 0xfffd, 0xfffd, 0xfffd, 0x5a, 0xd900, 0xdc00, 0x7a,
        0xd800, 0xdc00, 0xd840, 0xdc00, 0xdb40, 0xdc00, 0xdbff, 0xdfff
    };
    UnicodeString from32 = UnicodeString::fromUTF32(utf32, UPRV_LENGTHOF(utf32));
    UnicodeString expected(FALSE, expected_utf16, UPRV_LENGTHOF(expected_utf16));
    if(from32 != expected) {
        errln("UnicodeString::fromUTF32() did not create the expected string.");
    }

    static const UChar utf16[] = {
        0x41, 0xd900, 0x61, 0xdc00, 0x5a, 0xd900, 0xdc00, 0x7a, 0xd800, 0xdc00, 0xdbff, 0xdfff
    };
    static const UChar32 expected_utf32[] = {
        0x41, 0xfffd, 0x61, 0xfffd, 0x5a, 0x50000, 0x7a, 0x10000, 0x10ffff
    };
    UChar32 result32[16];
    UErrorCode errorCode = U_ZERO_ERROR;
    int32_t length32 =
        UnicodeString(FALSE, utf16, UPRV_LENGTHOF(utf16)).
        toUTF32(result32, UPRV_LENGTHOF(result32), errorCode);
    if( length32 != UPRV_LENGTHOF(expected_utf32) ||
        0 != uprv_memcmp(result32, expected_utf32, length32*4) ||
        result32[length32] != 0
    ) {
        errln("UnicodeString::toUTF32() did not create the expected string.");
    }
}
UnicodeStringTest::TestUTF8() {
    static const uint8_t utf8[] = {
        // Code points:
        // 0x41, 0xd900,
        // 0x61, 0xdc00,
        // 0x110000, 0x5a,
        // 0x50000, 0x7a,
        // 0x10000, 0x20000,
        // 0xe0000, 0x10ffff
        0x41, 0xed, 0xa4, 0x80,
        0x61, 0xed, 0xb0, 0x80,
        0xf4, 0x90, 0x80, 0x80, 0x5a,
        0xf1, 0x90, 0x80, 0x80, 0x7a,
        0xf0, 0x90, 0x80, 0x80, 0xf0, 0xa0, 0x80, 0x80,
        0xf3, 0xa0, 0x80, 0x80, 0xf4, 0x8f, 0xbf, 0xbf
    };
    static const UChar expected_utf16[] = {
        0x41, 0xfffd, 0xfffd, 0xfffd,
        0x61, 0xfffd, 0xfffd, 0xfffd,
        0xfffd,  0xfffd, 0xfffd, 0xfffd,0x5a,
        0xd900, 0xdc00, 0x7a,
        0xd800, 0xdc00, 0xd840, 0xdc00,
        0xdb40, 0xdc00, 0xdbff, 0xdfff
    };
    UnicodeString from8 = UnicodeString::fromUTF8(StringPiece((const char *)utf8, (int32_t)sizeof(utf8)));
    UnicodeString expected(FALSE, expected_utf16, UPRV_LENGTHOF(expected_utf16));

    if(from8 != expected) {
        errln("UnicodeString::fromUTF8(StringPiece) did not create the expected string.");
    }
    std::string utf8_string((const char *)utf8, sizeof(utf8));
    UnicodeString from8b = UnicodeString::fromUTF8(utf8_string);
    if(from8b != expected) {
        errln("UnicodeString::fromUTF8(std::string) did not create the expected string.");
    }

    static const UChar utf16[] = {
        0x41, 0xd900, 0x61, 0xdc00, 0x5a, 0xd900, 0xdc00, 0x7a, 0xd800, 0xdc00, 0xdbff, 0xdfff
    };
    static const uint8_t expected_utf8[] = {
        0x41, 0xef, 0xbf, 0xbd, 0x61, 0xef, 0xbf, 0xbd, 0x5a, 0xf1, 0x90, 0x80, 0x80, 0x7a,
        0xf0, 0x90, 0x80, 0x80, 0xf4, 0x8f, 0xbf, 0xbf
    };
    UnicodeString us(FALSE, utf16, UPRV_LENGTHOF(utf16));

    char buffer[64];
    TestCheckedArrayByteSink sink(buffer, (int32_t)sizeof(buffer));
    us.toUTF8(sink);
    if( sink.NumberOfBytesWritten() != (int32_t)sizeof(expected_utf8) ||
        0 != uprv_memcmp(buffer, expected_utf8, sizeof(expected_utf8))
    ) {
        errln("UnicodeString::toUTF8() did not create the expected string.");
    }
    if(!sink.calledFlush) {
        errln("UnicodeString::toUTF8(sink) did not sink.Flush().");
    }
    // Initial contents for testing that toUTF8String() appends.
    std::string result8 = "-->";
    std::string expected8 = "-->" + std::string((const char *)expected_utf8, sizeof(expected_utf8));
    // Use the return value just for testing.
    std::string &result8r = us.toUTF8String(result8);
    if(result8r != expected8 || &result8r != &result8) {
        errln("UnicodeString::toUTF8String() did not create the expected string.");
    }
}
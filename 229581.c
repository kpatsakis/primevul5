UnicodeStringTest::TestReverse()
{
    UnicodeString test("backwards words say to used I");

    test.reverse();
    test.reverse(2, 4);
    test.reverse(7, 2);
    test.reverse(10, 3);
    test.reverse(14, 5);
    test.reverse(20, 9);

    if (test != "I used to say words backwards")
        errln("reverse() failed:  Expected \"I used to say words backwards\",\n got \""
            + test + "\"");

    test=UNICODE_STRING("\\U0002f999\\U0001d15f\\u00c4\\u1ed0", 32).unescape();
    test.reverse();
    if(test.char32At(0)!=0x1ed0 || test.char32At(1)!=0xc4 || test.char32At(2)!=0x1d15f || test.char32At(4)!=0x2f999) {
        errln("reverse() failed with supplementary characters");
    }

    // Test case for ticket #8091:
    // UnicodeString::reverse() failed to see a lead surrogate in the middle of
    // an odd-length string that contains no other lead surrogates.
    test=UNICODE_STRING_SIMPLE("ab\\U0001F4A9e").unescape();
    UnicodeString expected=UNICODE_STRING_SIMPLE("e\\U0001F4A9ba").unescape();
    test.reverse();
    if(test!=expected) {
        errln("reverse() failed with only lead surrogate in the middle");
    }
}
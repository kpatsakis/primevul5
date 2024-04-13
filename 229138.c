TEST(BitTestMatchExpression, DoesNotMatchBinaryWithBitMask) {
    const char* bas = "\0\x03\x60\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xFF";
    const char* bac = "\0\xFD\x9F\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xFF";

    BSONObj match1 = fromjson("{a: {$binary: 'AANgAAAAAAAAAAAAAAAAAAAAAAAA', $type: '00'}}");
    // Base64 to Binary: 00000000 00000011 01100000
    BSONObj match2 = fromjson("{a: {$binary: 'JANgAwetkqwklEWRbWERKKJREtbq', $type: '00'}}");
    // Base64 to Binary: ........ 00000011 01100000

    BitsAllSetMatchExpression balls("a", bas, 22);
    BitsAllClearMatchExpression ballc("a", bac, 22);
    BitsAnySetMatchExpression banys("a", bas, 22);
    BitsAnyClearMatchExpression banyc("a", bac, 22);
    ASSERT(!balls.matchesSingleElement(match1["a"]));
    ASSERT(!balls.matchesSingleElement(match2["a"]));
    ASSERT(!ballc.matchesSingleElement(match1["a"]));
    ASSERT(!ballc.matchesSingleElement(match2["a"]));
    ASSERT(banys.matchesSingleElement(match1["a"]));
    ASSERT(banys.matchesSingleElement(match2["a"]));
    ASSERT(banyc.matchesSingleElement(match1["a"]));
    ASSERT(banyc.matchesSingleElement(match2["a"]));
}
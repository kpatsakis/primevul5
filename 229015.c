TEST(BitTestMatchExpression, MatchLongWithBinaryBitMask) {
    const char* bitMaskSet = "\x36\x00\x00\x00";
    const char* bitMaskClear = "\xC9\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF";

    BSONObj match = fromjson("{a: 54}");

    BitsAllSetMatchExpression balls("a", bitMaskSet, 4);
    BitsAllClearMatchExpression ballc("a", bitMaskClear, 9);
    BitsAnySetMatchExpression banys("a", bitMaskSet, 4);
    BitsAnyClearMatchExpression banyc("a", bitMaskClear, 9);

    ASSERT(balls.matchesSingleElement(match["a"]));
    ASSERT(ballc.matchesSingleElement(match["a"]));
    ASSERT(banys.matchesSingleElement(match["a"]));
    ASSERT(banyc.matchesSingleElement(match["a"]));
}
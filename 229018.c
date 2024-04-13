TEST(BitTestMatchExpression, MatchesNegativeIntegerWithBitMask) {
    long long bitMaskSet = 10;
    long long bitMaskClear = 5;

    BSONObj match1 = fromjson("{a: NumberInt(-54)}");
    BSONObj match2 = fromjson("{a: NumberLong(-54)}");
    BSONObj match3 = fromjson("{a: -54.0}");

    BitsAllSetMatchExpression balls("a", bitMaskSet);
    BitsAllClearMatchExpression ballc("a", bitMaskClear);
    BitsAnySetMatchExpression banys("a", bitMaskSet);
    BitsAnyClearMatchExpression banyc("a", bitMaskClear);

    ASSERT(balls.matchesSingleElement(match1["a"]));
    ASSERT(balls.matchesSingleElement(match2["a"]));
    ASSERT(balls.matchesSingleElement(match3["a"]));
    ASSERT(ballc.matchesSingleElement(match1["a"]));
    ASSERT(ballc.matchesSingleElement(match2["a"]));
    ASSERT(ballc.matchesSingleElement(match3["a"]));
    ASSERT(banys.matchesSingleElement(match1["a"]));
    ASSERT(banys.matchesSingleElement(match2["a"]));
    ASSERT(banys.matchesSingleElement(match3["a"]));
    ASSERT(banyc.matchesSingleElement(match1["a"]));
    ASSERT(banyc.matchesSingleElement(match2["a"]));
    ASSERT(banyc.matchesSingleElement(match3["a"]));
}
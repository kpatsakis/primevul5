TEST(BitTestMatchExpression, MatchesEmpty) {
    std::vector<uint32_t> bitPositions;

    BSONObj match1 = fromjson("{a: NumberInt(54)}");
    BSONObj match2 = fromjson("{a: NumberLong(54)}");
    BSONObj match3 = fromjson("{a: 54.0}");
    BSONObj match4 = fromjson("{a: {$binary: '2AAAAAAAAAAAAAAAAAAAAAAAAAAA', $type: '00'}}");

    BitsAllSetMatchExpression balls("a", bitPositions);
    BitsAllClearMatchExpression ballc("a", bitPositions);
    BitsAnySetMatchExpression banys("a", bitPositions);
    BitsAnyClearMatchExpression banyc("a", bitPositions);

    ASSERT_EQ((size_t)0, balls.numBitPositions());
    ASSERT_EQ((size_t)0, ballc.numBitPositions());
    ASSERT_EQ((size_t)0, banys.numBitPositions());
    ASSERT_EQ((size_t)0, banyc.numBitPositions());
    ASSERT(balls.matchesSingleElement(match1["a"]));
    ASSERT(balls.matchesSingleElement(match2["a"]));
    ASSERT(balls.matchesSingleElement(match3["a"]));
    ASSERT(balls.matchesSingleElement(match4["a"]));
    ASSERT(ballc.matchesSingleElement(match1["a"]));
    ASSERT(ballc.matchesSingleElement(match2["a"]));
    ASSERT(ballc.matchesSingleElement(match3["a"]));
    ASSERT(ballc.matchesSingleElement(match4["a"]));
    ASSERT(!banys.matchesSingleElement(match1["a"]));
    ASSERT(!banys.matchesSingleElement(match2["a"]));
    ASSERT(!banys.matchesSingleElement(match3["a"]));
    ASSERT(!banys.matchesSingleElement(match4["a"]));
    ASSERT(!banyc.matchesSingleElement(match1["a"]));
    ASSERT(!banyc.matchesSingleElement(match2["a"]));
    ASSERT(!banyc.matchesSingleElement(match3["a"]));
    ASSERT(!banyc.matchesSingleElement(match4["a"]));
}
TEST(BitTestMatchExpression, DoesNotMatchInteger) {
    BSONArray bas = BSON_ARRAY(1 << 2 << 4 << 5 << 6);
    BSONArray bac = BSON_ARRAY(0 << 3 << 1);
    std::vector<uint32_t> bitPositionsSet = bsonArrayToBitPositions(bas);
    std::vector<uint32_t> bitPositionsClear = bsonArrayToBitPositions(bac);

    BSONObj match1 = fromjson("{a: NumberInt(54)}");
    BSONObj match2 = fromjson("{a: NumberLong(54)}");
    BSONObj match3 = fromjson("{a: 54.0}");

    BitsAllSetMatchExpression balls("a", bitPositionsSet);
    BitsAllClearMatchExpression ballc("a", bitPositionsClear);
    BitsAnySetMatchExpression banys("a", bitPositionsSet);
    BitsAnyClearMatchExpression banyc("a", bitPositionsClear);

    ASSERT_EQ((size_t)5, balls.numBitPositions());
    ASSERT_EQ((size_t)3, ballc.numBitPositions());
    ASSERT_EQ((size_t)5, banys.numBitPositions());
    ASSERT_EQ((size_t)3, banyc.numBitPositions());
    ASSERT(!balls.matchesSingleElement(match1["a"]));
    ASSERT(!balls.matchesSingleElement(match2["a"]));
    ASSERT(!balls.matchesSingleElement(match3["a"]));
    ASSERT(!ballc.matchesSingleElement(match1["a"]));
    ASSERT(!ballc.matchesSingleElement(match2["a"]));
    ASSERT(!ballc.matchesSingleElement(match3["a"]));
    ASSERT(banys.matchesSingleElement(match1["a"]));
    ASSERT(banys.matchesSingleElement(match2["a"]));
    ASSERT(banys.matchesSingleElement(match3["a"]));
    ASSERT(banyc.matchesSingleElement(match1["a"]));
    ASSERT(banyc.matchesSingleElement(match2["a"]));
    ASSERT(banyc.matchesSingleElement(match3["a"]));
}
TEST(BitTestMatchExpression, MatchesBinary1) {
    BSONArray bas = BSON_ARRAY(1 << 2 << 4 << 5);
    BSONArray bac = BSON_ARRAY(0 << 3 << 600);
    std::vector<uint32_t> bitPositionsSet = bsonArrayToBitPositions(bas);
    std::vector<uint32_t> bitPositionsClear = bsonArrayToBitPositions(bac);

    BSONObj match1 = fromjson("{a: {$binary: 'NgAAAAAAAAAAAAAAAAAAAAAAAAAA', $type: '00'}}");
    // Base64 to Binary: 00110110...
    BSONObj match2 = fromjson("{a: {$binary: 'NgAjqwetkqwklEWRbWERKKJREtbq', $type: '00'}}");
    // Base64 to Binary: 00110110...

    BitsAllSetMatchExpression balls("a", bitPositionsSet);
    BitsAllClearMatchExpression ballc("a", bitPositionsClear);
    BitsAnySetMatchExpression banys("a", bitPositionsSet);
    BitsAnyClearMatchExpression banyc("a", bitPositionsClear);

    ASSERT_EQ((size_t)4, balls.numBitPositions());
    ASSERT_EQ((size_t)3, ballc.numBitPositions());
    ASSERT_EQ((size_t)4, banys.numBitPositions());
    ASSERT_EQ((size_t)3, banyc.numBitPositions());
    ASSERT(balls.matchesSingleElement(match1["a"]));
    ASSERT(balls.matchesSingleElement(match2["a"]));
    ASSERT(ballc.matchesSingleElement(match1["a"]));
    ASSERT(ballc.matchesSingleElement(match2["a"]));
    ASSERT(banys.matchesSingleElement(match1["a"]));
    ASSERT(banys.matchesSingleElement(match2["a"]));
    ASSERT(banyc.matchesSingleElement(match1["a"]));
    ASSERT(banyc.matchesSingleElement(match2["a"]));
}
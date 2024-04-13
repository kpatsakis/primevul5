TEST(BitTestMatchExpression, MatchBinaryWithLongBitMask) {
    long long bitMask = 54;

    BSONObj match = fromjson("{a: {$binary: 'NgAAAAAAAAAAAAAAAAAAAAAAAAAA', $type: '00'}}");

    BitsAllSetMatchExpression balls("a", bitMask);
    BitsAllClearMatchExpression ballc("a", bitMask);
    BitsAnySetMatchExpression banys("a", bitMask);
    BitsAnyClearMatchExpression banyc("a", bitMask);

    std::vector<uint32_t> bitPositions = balls.getBitPositions();
    ASSERT(balls.matchesSingleElement(match["a"]));
    ASSERT(!ballc.matchesSingleElement(match["a"]));
    ASSERT(banys.matchesSingleElement(match["a"]));
    ASSERT(!banyc.matchesSingleElement(match["a"]));
}
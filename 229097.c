TEST(BitTestMatchExpression, DoesNotMatchOther) {
    std::vector<uint32_t> bitPositions;

    BSONObj notMatch1 = fromjson("{a: {}}");     // Object
    BSONObj notMatch2 = fromjson("{a: null}");   // Null
    BSONObj notMatch3 = fromjson("{a: []}");     // Array
    BSONObj notMatch4 = fromjson("{a: true}");   // Boolean
    BSONObj notMatch5 = fromjson("{a: ''}");     // String
    BSONObj notMatch6 = fromjson("{a: 5.5}");    // Non-integral Double
    BSONObj notMatch7 = fromjson("{a: NaN}");    // NaN
    BSONObj notMatch8 = fromjson("{a: 1e100}");  // Too-Large Double
    BSONObj notMatch9 = fromjson("{a: ObjectId('000000000000000000000000')}");  // OID
    BSONObj notMatch10 = fromjson("{a: Date(54)}");                             // Date

    BitsAllSetMatchExpression balls("a", bitPositions);
    BitsAllClearMatchExpression ballc("a", bitPositions);
    BitsAnySetMatchExpression banys("a", bitPositions);
    BitsAnyClearMatchExpression banyc("a", bitPositions);

    ASSERT_EQ((size_t)0, balls.numBitPositions());
    ASSERT_EQ((size_t)0, ballc.numBitPositions());
    ASSERT_EQ((size_t)0, banys.numBitPositions());
    ASSERT_EQ((size_t)0, banyc.numBitPositions());
    ASSERT(!balls.matchesSingleElement(notMatch1["a"]));
    ASSERT(!balls.matchesSingleElement(notMatch2["a"]));
    ASSERT(!balls.matchesSingleElement(notMatch3["a"]));
    ASSERT(!balls.matchesSingleElement(notMatch4["a"]));
    ASSERT(!balls.matchesSingleElement(notMatch5["a"]));
    ASSERT(!balls.matchesSingleElement(notMatch6["a"]));
    ASSERT(!balls.matchesSingleElement(notMatch7["a"]));
    ASSERT(!balls.matchesSingleElement(notMatch8["a"]));
    ASSERT(!balls.matchesSingleElement(notMatch9["a"]));
    ASSERT(!balls.matchesSingleElement(notMatch10["a"]));
    ASSERT(!ballc.matchesSingleElement(notMatch1["a"]));
    ASSERT(!ballc.matchesSingleElement(notMatch2["a"]));
    ASSERT(!ballc.matchesSingleElement(notMatch3["a"]));
    ASSERT(!ballc.matchesSingleElement(notMatch4["a"]));
    ASSERT(!ballc.matchesSingleElement(notMatch5["a"]));
    ASSERT(!ballc.matchesSingleElement(notMatch6["a"]));
    ASSERT(!ballc.matchesSingleElement(notMatch7["a"]));
    ASSERT(!ballc.matchesSingleElement(notMatch8["a"]));
    ASSERT(!ballc.matchesSingleElement(notMatch9["a"]));
    ASSERT(!ballc.matchesSingleElement(notMatch10["a"]));
    ASSERT(!banys.matchesSingleElement(notMatch1["a"]));
    ASSERT(!banys.matchesSingleElement(notMatch2["a"]));
    ASSERT(!banys.matchesSingleElement(notMatch3["a"]));
    ASSERT(!banys.matchesSingleElement(notMatch4["a"]));
    ASSERT(!banys.matchesSingleElement(notMatch5["a"]));
    ASSERT(!banys.matchesSingleElement(notMatch6["a"]));
    ASSERT(!banys.matchesSingleElement(notMatch7["a"]));
    ASSERT(!banys.matchesSingleElement(notMatch8["a"]));
    ASSERT(!banys.matchesSingleElement(notMatch9["a"]));
    ASSERT(!banys.matchesSingleElement(notMatch10["a"]));
    ASSERT(!banyc.matchesSingleElement(notMatch1["a"]));
    ASSERT(!banyc.matchesSingleElement(notMatch2["a"]));
    ASSERT(!banyc.matchesSingleElement(notMatch3["a"]));
    ASSERT(!banyc.matchesSingleElement(notMatch4["a"]));
    ASSERT(!banyc.matchesSingleElement(notMatch5["a"]));
    ASSERT(!banyc.matchesSingleElement(notMatch6["a"]));
    ASSERT(!banyc.matchesSingleElement(notMatch7["a"]));
    ASSERT(!banyc.matchesSingleElement(notMatch8["a"]));
    ASSERT(!banyc.matchesSingleElement(notMatch9["a"]));
    ASSERT(!banyc.matchesSingleElement(notMatch10["a"]));
}
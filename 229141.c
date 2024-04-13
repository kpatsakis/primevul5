TEST_F(QueryPlannerTest, MaxMinBadHintSelectsReverseIndex) {
    // There are both ascending and descending indices on 'a'.
    addIndex(BSON("a" << 1));
    addIndex(BSON("a" << -1));

    // A query hinting on {a: 1} is bad if min is {a: 8} and {a: 2} because this
    // min/max pairing requires a descending index.
    runInvalidQueryFull(BSONObj(),
                        BSONObj(),
                        BSONObj(),
                        0,
                        0,
                        fromjson("{a: 1}"),
                        fromjson("{a: 8}"),
                        fromjson("{a: 2}"));
}
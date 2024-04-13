TEST_F(QueryPlannerTest, OrEnumerationLimit) {
    params.options = QueryPlannerParams::NO_TABLE_SCAN;
    addIndex(BSON("a" << 1));
    addIndex(BSON("b" << 1));

    // 6 $or clauses, each with 2 indexed predicates
    // means 2^6 = 64 possibilities. We should hit the limit.
    runQuery(
        fromjson("{$or: [{a: 1, b: 1},"
                 "{a: 2, b: 2},"
                 "{a: 3, b: 3},"
                 "{a: 4, b: 4},"
                 "{a: 5, b: 5},"
                 "{a: 6, b: 6}]}"));

    assertNumSolutions(internalQueryEnumerationMaxOrSolutions.load());
}
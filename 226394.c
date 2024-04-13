TEST_F(QueryPlannerTest, IntersectCanBeVeryBig) {
    params.options = QueryPlannerParams::NO_TABLE_SCAN | QueryPlannerParams::INDEX_INTERSECTION;
    addIndex(BSON("a" << 1));
    addIndex(BSON("b" << 1));
    addIndex(BSON("c" << 1));
    addIndex(BSON("d" << 1));
    runQuery(
        fromjson("{$or: [{ 'a' : null, 'b' : 94, 'c' : null, 'd' : null },"
                 "{ 'a' : null, 'b' : 98, 'c' : null, 'd' : null },"
                 "{ 'a' : null, 'b' : 1, 'c' : null, 'd' : null },"
                 "{ 'a' : null, 'b' : 2, 'c' : null, 'd' : null },"
                 "{ 'a' : null, 'b' : 7, 'c' : null, 'd' : null },"
                 "{ 'a' : null, 'b' : 9, 'c' : null, 'd' : null },"
                 "{ 'a' : null, 'b' : 16, 'c' : null, 'd' : null }]}"));

    assertNumSolutions(internalQueryEnumerationMaxOrSolutions.load());
}
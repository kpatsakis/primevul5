TEST_F(QueryPlannerTest, TwoPlans) {
    addIndex(BSON("a" << 1));
    addIndex(BSON("a" << 1 << "b" << 1));

    runQuery(fromjson("{a:1, b:{$gt:2,$lt:2}}"));

    // 2 indexed solns and one non-indexed
    ASSERT_EQUALS(getNumSolutions(), 3U);
    assertSolutionExists("{cscan: {dir: 1, filter: {$and:[{b:{$lt:2}},{a:1},{b:{$gt:2}}]}}}");
    assertSolutionExists(
        "{fetch: {filter: {$and:[{b:{$lt:2}},{b:{$gt:2}}]}, node: "
        "{ixscan: {filter: null, pattern: {a: 1}}}}}");
    assertSolutionExists(
        "{fetch: {filter: null, node: {ixscan: "
        "{filter: null, pattern: {a: 1, b: 1}}}}}");
}
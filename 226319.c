TEST_F(QueryPlannerTest, AndWithUnindexedOrChild) {
    addIndex(BSON("a" << 1));
    runQuery(fromjson("{a:20, $or: [{b:1}, {c:7}]}"));

    ASSERT_EQUALS(getNumSolutions(), 2U);
    assertSolutionExists("{cscan: {dir: 1}}");

    // Logical rewrite means we could get one of these two outcomes:
    size_t matches = 0;
    matches += numSolutionMatches(
        "{fetch: {filter: {$or: [{b: 1}, {c: 7}]}, node: "
        "{ixscan: {filter: null, pattern: {a: 1}}}}}");
    matches += numSolutionMatches(
        "{or: {filter: null, nodes: ["
        "{fetch: {filter: {b:1}, node: {"
        "ixscan: {filter: null, pattern: {a:1}}}}},"
        "{fetch: {filter: {c:7}, node: {"
        "ixscan: {filter: null, pattern: {a:1}}}}}]}}");
    ASSERT_GREATER_THAN_OR_EQUALS(matches, 1U);
}
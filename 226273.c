TEST_F(QueryPlannerTest, AndWithOrWithOneIndex) {
    addIndex(BSON("b" << 1));
    addIndex(BSON("a" << 1));
    runQuery(fromjson("{$or: [{b:1}, {c:7}], a:20}"));

    // Logical rewrite gives us at least one of these:
    assertSolutionExists("{cscan: {dir: 1}}");
    size_t matches = 0;
    matches += numSolutionMatches(
        "{fetch: {filter: {$or: [{b: 1}, {c: 7}]}, "
        "node: {ixscan: {filter: null, pattern: {a: 1}}}}}");
    matches += numSolutionMatches(
        "{or: {filter: null, nodes: ["
        "{fetch: {filter: {b:1}, node: {"
        "ixscan: {filter: null, pattern: {a:1}}}}},"
        "{fetch: {filter: {c:7}, node: {"
        "ixscan: {filter: null, pattern: {a:1}}}}}]}}");
    ASSERT_GREATER_THAN_OR_EQUALS(matches, 1U);
}
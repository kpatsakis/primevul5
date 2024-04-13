TEST_F(QueryPlannerTest, AndSortedRequiresKeepMutations) {
    params.options = QueryPlannerParams::KEEP_MUTATIONS;
    params.options |= QueryPlannerParams::INDEX_INTERSECTION;

    addIndex(BSON("a" << 1));
    addIndex(BSON("b" << 1));
    runQuery(fromjson("{a: 2, b: 3}"));

    assertNumSolutions(3U);
    assertSolutionExists("{fetch: {filter: {a: 2}, node: {ixscan: {pattern: {b: 1}}}}}");
    assertSolutionExists("{fetch: {filter: {b: 3}, node: {ixscan: {pattern: {a: 1}}}}}");
    assertSolutionExists(
        "{fetch: {filter: null, node: {keep: {node: {andSorted: {nodes: ["
        "{ixscan: {pattern: {a: 1}}},"
        "{ixscan: {pattern: {b: 1}}}]}}}}}}");
}
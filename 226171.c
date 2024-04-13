TEST_F(QueryPlannerTest, AndHashRequiresKeepMutations) {
    params.options = QueryPlannerParams::KEEP_MUTATIONS;
    params.options |= QueryPlannerParams::INDEX_INTERSECTION;

    addIndex(BSON("a" << 1));
    addIndex(BSON("b" << 1));
    runQuery(fromjson("{a: {$gte: 0}, b: {$gte: 0}}"));

    assertNumSolutions(3U);
    assertSolutionExists("{fetch: {filter: {a: {$gte: 0}}, node: {ixscan: {pattern: {b: 1}}}}}");
    assertSolutionExists("{fetch: {filter: {b: {$gte: 0}}, node: {ixscan: {pattern: {a: 1}}}}}");
    assertSolutionExists(
        "{fetch: {filter: null, node: {keep: {node: {andHash: {nodes: ["
        "{ixscan: {pattern: {a: 1}}},"
        "{ixscan: {pattern: {b: 1}}}]}}}}}}");
}
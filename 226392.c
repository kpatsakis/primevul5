TEST_F(QueryPlannerTest, InBasicOrEquivalent) {
    addIndex(fromjson("{a: 1}"));
    runQuery(fromjson("{$or: [{a: 1}, {a: 2}]}"));

    assertNumSolutions(2U);
    assertSolutionExists("{cscan: {dir: 1, filter: {$or: [{a: 1}, {a: 2}]}}}");
    assertSolutionExists(
        "{fetch: {filter: null, "
        "node: {ixscan: {pattern: {a: 1}}}}}");
}
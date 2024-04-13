TEST_F(QueryPlannerTest, InCompoundIndexFirst) {
    addIndex(fromjson("{a: 1, b: 1}"));
    runQuery(fromjson("{a: {$in: [1, 2]}, b: 3}"));

    assertNumSolutions(2U);
    assertSolutionExists("{cscan: {dir: 1, filter: {b: 3, a: {$in: [1, 2]}}}}");
    assertSolutionExists(
        "{fetch: {filter: null, "
        "node: {ixscan: {pattern: {a: 1, b: 1}}}}}");
}
TEST_F(QueryPlannerTest, OrInexactCoveredMultikey) {
    // true means multikey
    addIndex(BSON("names" << 1), true);
    runQuery(fromjson("{$or: [{names: 'dave'}, {names: /joe/}]}"));

    assertNumSolutions(2U);
    assertSolutionExists("{cscan: {dir: 1}}");
    assertSolutionExists(
        "{fetch: {filter: {$or: [{names: 'dave'}, {names: /joe/}]}, "
        "node: {ixscan: {filter: null, pattern: {names: 1}}}}}");
}
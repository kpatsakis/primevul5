TEST_F(QueryPlannerTest, OrBelowElemMatchInexactCovered) {
    // true means multikey
    addIndex(BSON("a.b" << 1), true);
    runQuery(fromjson("{a: {$elemMatch: {$or: [{b: 'x'}, {b: /z/}]}}}"));

    assertNumSolutions(2U);
    assertSolutionExists("{cscan: {dir: 1}}");
    assertSolutionExists(
        "{fetch: {filter: {a: {$elemMatch: {$or: [{b: 'x'}, {b: /z/}]}}},"
        "node: {ixscan: {filter: null, pattern: {'a.b': 1}}}}}");
}
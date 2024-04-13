TEST_F(QueryPlannerTest, OrInexactWithExact2) {
    addIndex(BSON("a" << 1));
    addIndex(BSON("b" << 1));
    runQuery(fromjson("{$or: [{a: 'foo'}, {a: /bar/}, {b: 'foo'}, {b: /bar/}]}"));

    assertNumSolutions(2U);
    assertSolutionExists("{cscan: {dir: 1}}");
    assertSolutionExists(
        "{fetch: {node: {or: {nodes: ["
        "{ixscan: {filter: {$or:[{a:'foo'},{a:/bar/}]},"
        "pattern: {a: 1}}},"
        "{ixscan: {filter: {$or:[{b:'foo'},{b:/bar/}]},"
        "pattern: {b: 1}}}]}}}}");
}
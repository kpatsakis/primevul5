TEST_F(QueryPlannerTest, RootedOrOfAndDontCollapseDifferentBounds) {
    addIndex(BSON("a" << 1 << "b" << 1));
    addIndex(BSON("c" << 1 << "d" << 1));
    runQuery(fromjson("{$or: [{a: 1, b: 2}, {c: 3, d: 4}, {a: 1, b: 99}]}"));

    assertNumSolutions(2U);
    assertSolutionExists("{cscan: {dir: 1}}");
    assertSolutionExists(
        "{fetch: {filter: null, node: {or: {nodes: ["
        "{ixscan: {pattern: {a: 1, b: 1}, filter: null,"
        "bounds: {a: [[1,1,true,true]], b: [[2,2,true,true]]}}},"
        "{ixscan: {pattern: {a: 1, b: 1}, filter: null,"
        "bounds: {a: [[1,1,true,true]], b: [[99,99,true,true]]}}},"
        "{ixscan: {pattern: {c: 1, d: 1}, filter: null,"
        "bounds: {c: [[3,3,true,true]], d: [[4,4,true,true]]}}}]}}}}");
}
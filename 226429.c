TEST_F(QueryPlannerTest, RootedOrOfAndCollapseTwoScansButNotThirdWithFilters) {
    addIndex(BSON("a" << 1 << "b" << 1));
    addIndex(BSON("c" << 1 << "d" << 1));
    runQuery(fromjson("{$or: [{a:1, b:2, e:5}, {c:3, d:4}, {a:1, b:2, f:6}]}"));

    assertNumSolutions(2U);
    assertSolutionExists("{cscan: {dir: 1}}");
    assertSolutionExists(
        "{fetch: {filter: null, node: {or: {nodes: ["
        "{fetch: {filter: {$or: [{f:6},{e:5}]}, node: "
        "{ixscan: {pattern: {a: 1, b: 1}, filter: null,"
        "bounds: {a: [[1,1,true,true]], b: [[2,2,true,true]]}}}}},"
        "{ixscan: {pattern: {c: 1, d: 1}, filter: null,"
        "bounds: {c: [[3,3,true,true]], d: [[4,4,true,true]]}}}]}}}}");
}
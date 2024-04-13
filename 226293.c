TEST_F(QueryPlannerTest, OrOfAnd) {
    addIndex(BSON("a" << 1));
    runQuery(fromjson("{$or: [{a:{$gt:2,$lt:10}}, {a:{$gt:0,$lt:5}}]}"));

    assertNumSolutions(2U);
    assertSolutionExists("{cscan: {dir: 1}}");
    assertSolutionExists(
        "{fetch: {filter: null, node: {or: {nodes: ["
        "{ixscan: {pattern: {a:1}, bounds: {a: [[2,10,false,false]]}}}, "
        "{ixscan: {pattern: {a:1}, bounds: "
        "{a: [[0,5,false,false]]}}}]}}}}");
}
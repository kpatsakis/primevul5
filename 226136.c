TEST_F(QueryPlannerTest, OrOfAnd2) {
    addIndex(BSON("a" << 1));
    runQuery(fromjson("{$or: [{a:{$gt:2,$lt:10}}, {a:{$gt:0,$lt:15}}, {a:{$gt:20}}]}"));

    assertNumSolutions(2U);
    assertSolutionExists("{cscan: {dir: 1}}");
    assertSolutionExists(
        "{fetch: {filter: null, node: {or: {nodes: ["
        "{ixscan: {pattern: {a:1}, bounds: {a: [[2,10,false,false]]}}}, "
        "{ixscan: {pattern: {a:1}, bounds: {a: [[0,15,false,false]]}}}, "
        "{ixscan: {pattern: {a:1}, bounds: "
        "{a: [[20,Infinity,false,true]]}}}]}}}}");
}
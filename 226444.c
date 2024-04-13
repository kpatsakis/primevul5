TEST_F(QueryPlannerTest, OrOfAnd3) {
    addIndex(BSON("a" << 1));
    runQuery(fromjson("{$or: [{a:{$gt:1,$lt:5},b:6}, {a:3,b:{$gt:0,$lt:10}}]}"));

    assertNumSolutions(2U);
    assertSolutionExists("{cscan: {dir: 1}}");
    assertSolutionExists(
        "{or: {nodes: ["
        "{fetch: {filter: {b:6}, node: {ixscan: {pattern: {a:1}, "
        "bounds: {a: [[1,5,false,false]]}}}}}, "
        "{fetch: {filter: {$and:[{b:{$lt:10}},{b:{$gt:0}}]}, node: "
        "{ixscan: {pattern: {a:1}, bounds: {a:[[3,3,true,true]]}}}}}]}}");
}
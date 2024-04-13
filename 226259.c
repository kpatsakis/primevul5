TEST_F(QueryPlannerTest, OrOfAnd4) {
    addIndex(BSON("a" << 1 << "b" << 1));
    runQuery(
        fromjson("{$or: [{a:{$gt:1,$lt:5}, b:{$gt:0,$lt:3}, c:6}, "
                 "{a:3, b:{$gt:1,$lt:2}, c:{$gt:0,$lt:10}}]}"));

    assertNumSolutions(2U);
    assertSolutionExists("{cscan: {dir: 1}}");
    assertSolutionExists(
        "{or: {nodes: ["
        "{fetch: {filter: {c:6}, node: {ixscan: {pattern: {a:1,b:1}, "
        "bounds: {a: [[1,5,false,false]], b: [[0,3,false,false]]}}}}}, "
        "{fetch: {filter: {$and:[{c:{$lt:10}},{c:{$gt:0}}]}, node: "
        "{ixscan: {pattern: {a:1,b:1}, "
        " bounds: {a:[[3,3,true,true]], b:[[1,2,false,false]]}}}}}]}}");
}
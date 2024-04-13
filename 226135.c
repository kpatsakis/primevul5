TEST_F(QueryPlannerTest, OrOfAnd6) {
    addIndex(BSON("a" << 1 << "b" << 1));
    runQuery(fromjson("{$or: [{a:{$in:[1]},b:{$in:[1]}}, {a:{$in:[1,5]},b:{$in:[1,5]}}]}"));

    assertNumSolutions(2U);
    assertSolutionExists("{cscan: {dir: 1}}");
    assertSolutionExists(
        "{fetch: {filter: null, node: {or: {nodes: ["
        "{ixscan: {pattern: {a:1,b:1}, bounds: "
        "{a: [[1,1,true,true]], b: [[1,1,true,true]]}}}, "
        "{ixscan: {pattern: {a:1,b:1}, bounds: "
        "{a: [[1,1,true,true], [5,5,true,true]], "
        " b: [[1,1,true,true], [5,5,true,true]]}}}]}}}}");
}
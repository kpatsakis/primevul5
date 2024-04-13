TEST_F(QueryPlannerTest, TwoPlansElemMatch) {
    addIndex(BSON("a" << 1 << "b" << 1));
    addIndex(BSON("arr.x" << 1 << "a" << 1));

    runQuery(
        fromjson("{arr: { $elemMatch : { x : 5 , y : 5 } },"
                 " a : 55 , b : { $in : [ 1 , 5 , 8 ] } }"));

    // 2 indexed solns and one non-indexed
    ASSERT_EQUALS(getNumSolutions(), 3U);
    assertSolutionExists("{cscan: {dir: 1}}");
    assertSolutionExists(
        "{fetch: {node: {ixscan: {pattern: {a: 1, b: 1}, bounds: "
        "{a: [[55,55,true,true]], b: [[1,1,true,true], "
        "[5,5,true,true], [8,8,true,true]]}}}}}");
    assertSolutionExists(
        "{fetch: {filter: {$and: [{arr:{$elemMatch:{x:5,y:5}}},"
        "{b:{$in:[1,5,8]}}]}, "
        "node: {ixscan: {pattern: {'arr.x':1,a:1}, bounds: "
        "{'arr.x': [[5,5,true,true]], 'a':[[55,55,true,true]]}}}}}");
}
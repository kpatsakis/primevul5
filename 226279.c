TEST_F(QueryPlannerTest, CompoundIndexBoundsIntersectRanges) {
    addIndex(BSON("a" << 1 << "b" << 1 << "c" << 1));
    addIndex(BSON("a" << 1 << "c" << 1));
    runQuery(fromjson("{a: {$gt: 1, $lt: 10}, c: {$gt: 1, $lt: 10}}"));

    assertNumSolutions(3U);
    assertSolutionExists("{cscan: {dir: 1}}");
    assertSolutionExists(
        "{fetch: {filter: null, node: {ixscan: {pattern: {a:1,b:1,c:1}, "
        "bounds: {a: [[1,10,false,false]], "
        "b: [['MinKey','MaxKey',true,true]], "
        "c: [[1,10,false,false]]}}}}}");
    assertSolutionExists(
        "{fetch: {filter: null, node: {ixscan: {pattern: {a:1,c:1}, "
        "bounds: {a: [[1,10,false,false]], "
        "c: [[1,10,false,false]]}}}}}");
}
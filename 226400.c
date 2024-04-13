TEST_F(QueryPlannerTest, CompoundMultikeyBoundsNoIntersect) {
    // true means multikey
    addIndex(BSON("a" << 1 << "b" << 1), true);
    runQuery(fromjson("{a: 1, b: {$gt: 3, $lte: 5}}"));

    assertNumSolutions(2U);
    assertSolutionExists("{cscan: {dir: 1}}");
    assertSolutionExists(
        "{fetch: {filter: {b:{$gt:3}}, node: {ixscan: {filter: null, "
        "pattern: {a:1,b:1}, bounds: "
        "{a: [[1,1,true,true]], b: [[-Infinity,5,true,true]]}}}}}");
}
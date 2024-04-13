TEST_F(QueryPlannerTest, CompoundMultikeyBounds) {
    // true means multikey
    addIndex(BSON("a" << 1 << "b" << 1), true);
    runQuery(fromjson("{a: 1, b: 3}"));

    assertNumSolutions(2U);
    assertSolutionExists("{cscan: {filter: {$and:[{a:1},{b:3}]}, dir: 1}}");
    assertSolutionExists(
        "{fetch: {filter: null, node: {ixscan: {filter: null, "
        "pattern: {a:1,b:1}, bounds: "
        "{a: [[1,1,true,true]], b: [[3,3,true,true]]}}}}}");
}
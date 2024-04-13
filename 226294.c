TEST_F(QueryPlannerTest, NinUsesMultikeyIndex) {
    // true means multikey
    addIndex(BSON("a" << 1), true);
    runQuery(fromjson("{a: {$nin: [4, 10]}}"));

    assertNumSolutions(2U);
    assertSolutionExists("{cscan: {dir: 1}}");
    assertSolutionExists(
        "{fetch: {filter: {a:{$nin:[4,10]}}, node: {ixscan: {pattern: {a:1}, "
        "bounds: {a: [['MinKey',4,true,false],"
        "[4,10,false,false],"
        "[10,'MaxKey',false,true]]}}}}}");
}
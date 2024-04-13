TEST_F(QueryPlannerTest, NEOnMultikeyIndex) {
    // true means multikey
    addIndex(BSON("a" << 1), true);
    runQuery(fromjson("{a: {$ne: 3}}"));

    assertNumSolutions(2U);
    assertSolutionExists("{cscan: {dir: 1}}");
    assertSolutionExists(
        "{fetch: {filter: {a:{$ne:3}}, node: {ixscan: {pattern: {a:1}, "
        "bounds: {a: [['MinKey',3,true,false],"
        "[3,'MaxKey',false,true]]}}}}}");
}
TEST_F(QueryPlannerTest, NegatePredOnCompoundIndex) {
    addIndex(BSON("x" << 1 << "a" << 1));
    runQuery(fromjson("{x: 1, a: {$ne: 1}, b: {$ne: 2}}"));

    assertNumSolutions(2U);
    assertSolutionExists("{cscan: {dir: 1}}");
    assertSolutionExists(
        "{fetch: {node: {ixscan: {pattern: {x:1,a:1}, bounds: "
        "{x: [[1,1,true,true]], "
        "a: [['MinKey',1,true,false], [1,'MaxKey',false,true]]}}}}}");
}
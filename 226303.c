TEST_F(QueryPlannerTest, CompoundIndexBoundsStringBounds) {
    addIndex(BSON("a" << 1 << "b" << 1));
    runQuery(fromjson("{a: {$gt: 'foo'}, b: {$gte: 'bar'}}"));

    assertNumSolutions(2U);
    assertSolutionExists("{cscan: {dir: 1}}");
    assertSolutionExists(
        "{fetch: {filter: null, node: {ixscan: {filter: null, pattern: "
        "{a: 1, b: 1}, bounds: {a: [['foo',{},false,false]], "
        "b:[['bar',{},true,false]]}}}}}");
}
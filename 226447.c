TEST_F(QueryPlannerTest, ExistsBoundsCompound) {
    addIndex(BSON("a" << 1 << "b" << 1));

    runQuery(fromjson("{a: 1, b: {$exists: true}}"));
    assertNumSolutions(2U);
    assertSolutionExists("{cscan: {dir: 1}}");
    assertSolutionExists(
        "{fetch: {filter: {b: {$exists: true}}, node: "
        "{ixscan: {pattern: {a: 1, b: 1}, bounds: "
        "{a: [[1,1,true,true]], b: [['MinKey','MaxKey',true,true]]}}}}}");

    // This ends up being a double negation, which we currently don't index.
    runQuery(fromjson("{a: 1, b: {$not: {$exists: false}}}"));
    assertNumSolutions(2U);
    assertSolutionExists("{cscan: {dir: 1}}");
    assertSolutionExists(
        "{fetch: {node: {ixscan: {pattern: {a: 1, b: 1}, bounds: "
        "{a: [[1,1,true,true]], b: [['MinKey','MaxKey',true,true]]}}}}}");

    runQuery(fromjson("{a: 1, b: {$exists: false}}"));
    assertNumSolutions(2U);
    assertSolutionExists("{cscan: {dir: 1}}");
    assertSolutionExists(
        "{fetch: {filter: {b: {$exists: false}}, node: "
        "{ixscan: {pattern: {a: 1, b: 1}, bounds: "
        "{a: [[1,1,true,true]], b: [[null,null,true,true]]}}}}}");

    runQuery(fromjson("{a: 1, b: {$not: {$exists: true}}}"));
    assertNumSolutions(2U);
    assertSolutionExists("{cscan: {dir: 1}}");
    assertSolutionExists(
        "{fetch: {filter: {b: {$exists: false}}, node: "
        "{ixscan: {pattern: {a: 1, b: 1}, bounds: "
        "{a: [[1,1,true,true]], b: [[null,null,true,true]]}}}}}");
}
TEST_F(QueryPlannerTest, ExistsBounds) {
    addIndex(BSON("b" << 1));

    runQuery(fromjson("{b: {$exists: true}}"));
    assertNumSolutions(2U);
    assertSolutionExists("{cscan: {dir: 1}}");
    assertSolutionExists(
        "{fetch: {filter: {b: {$exists: true}}, node: "
        "{ixscan: {pattern: {b: 1}, bounds: "
        "{b: [['MinKey', 'MaxKey', true, true]]}}}}}");

    // This ends up being a double negation, which we currently don't index.
    runQuery(fromjson("{b: {$not: {$exists: false}}}"));
    assertNumSolutions(1U);
    assertSolutionExists("{cscan: {dir: 1}}");

    runQuery(fromjson("{b: {$exists: false}}"));
    assertNumSolutions(2U);
    assertSolutionExists("{cscan: {dir: 1}}");
    assertSolutionExists(
        "{fetch: {filter: {b: {$exists: false}}, node: "
        "{ixscan: {pattern: {b: 1}, bounds: "
        "{b: [[null, null, true, true]]}}}}}");

    runQuery(fromjson("{b: {$not: {$exists: true}}}"));
    assertNumSolutions(2U);
    assertSolutionExists("{cscan: {dir: 1}}");
    assertSolutionExists(
        "{fetch: {filter: {b: {$exists: false}}, node: "
        "{ixscan: {pattern: {b: 1}, bounds: "
        "{b: [[null, null, true, true]]}}}}}");
}
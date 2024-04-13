TEST_F(QueryPlannerTest, UniqueIndexLookupBelowOr) {
    params.options = QueryPlannerParams::NO_TABLE_SCAN;

    addIndex(BSON("a" << 1));
    addIndex(BSON("b" << 1));
    addIndex(BSON("c" << 1));
    addIndex(BSON("d" << 1),
             false,  // multikey
             false,  // sparse,
             true);  // unique

    runQuery(fromjson("{$or: [{a: 1, b: 1}, {c: 1, d: 1}]}"));

    // Only two plans because we throw out plans for the right branch of the $or that do not
    // use equality over the unique index.
    assertNumSolutions(2U);
    assertSolutionExists(
        "{or: {nodes: ["
        "{fetch: {filter: {a: 1}, node: {ixscan: {pattern: {b: 1}}}}},"
        "{fetch: {filter: {c: 1}, node: {ixscan: {pattern: {d: 1}}}}}]}}");
    assertSolutionExists(
        "{or: {nodes: ["
        "{fetch: {filter: {b: 1}, node: {ixscan: {pattern: {a: 1}}}}},"
        "{fetch: {filter: {c: 1}, node: {ixscan: {pattern: {d: 1}}}}}]}}");
}
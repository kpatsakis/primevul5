TEST_F(QueryPlannerTest, NestedContainedOrOneChildUsesPredicate) {
    addIndex(BSON("c" << 1 << "a" << 1));
    addIndex(BSON("d" << 1));
    addIndex(BSON("f" << 1));
    addIndex(BSON("g" << 1 << "a" << 1));

    runQuery(
        fromjson("{$and: [{a: 5}, {$or: [{$and: [{b: 6}, {$or: [{c: 7}, {d: 8}]}]}, "
                 "{$and: [{e: 9}, {$or: [{f: 10}, {g: 11}]}]}]}]}"));
    assertNumSolutions(2);
    assertSolutionExists(
        "{fetch: {filter: {a: 5}, node: {or: {nodes: ["
        "{fetch: {filter: {b: 6}, node: {or: {nodes: ["
        "{ixscan: {pattern: {c: 1, a: 1}, bounds: {c: [[7, 7, true, true]], a: [[5, 5, true, "
        "true]]}}},"
        "{ixscan: {pattern: {d: 1}, bounds: {d: [[8, 8, true, true]]}}}"
        "]}}}},"
        "{fetch: {filter: {e: 9}, node: {or: {nodes: ["
        "{ixscan: {pattern: {f: 1}, bounds: {f: [[10, 10, true, true]]}}},"
        "{ixscan: {pattern: {g: 1, a: 1}, bounds: {g: [[11, 11, true, true]], a: [[5, 5, true, "
        "true]]}}}"
        "]}}}}"
        "]}}}}");
    assertSolutionExists("{cscan: {dir: 1}}}}");
}
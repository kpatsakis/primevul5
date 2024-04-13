TEST_F(QueryPlannerTest, ThreeRegexSameFieldMultikey) {
    // true means multikey
    addIndex(BSON("a" << 1), true);
    runQuery(fromjson("{$and: [{a: /0/}, {a: /1/}, {a: /2/}]}"));

    ASSERT_EQUALS(getNumSolutions(), 4U);
    assertSolutionExists("{cscan: {filter: {$and:[{a:/0/},{a:/1/},{a:/2/}]}, dir: 1}}");
    assertSolutionExists(
        "{fetch: {filter: {$and:[{a:/0/},{a:/1/},{a:/2/}]}, node: {ixscan: "
        "{pattern: {a: 1}, filter: null, "
        "bounds: {a: [['', {}, true, false], [/0/, /0/, true, true]]}}}}}");
    assertSolutionExists(
        "{fetch: {filter: {$and:[{a:/1/},{a:/0/},{a:/2/}]}, node: {ixscan: "
        "{pattern: {a: 1}, filter: null, "
        "bounds: {a: [['', {}, true, false], [/1/, /1/, true, true]]}}}}}");
    assertSolutionExists(
        "{fetch: {filter: {$and:[{a:/2/},{a:/0/},{a:/1/}]}, node: {ixscan: "
        "{pattern: {a: 1}, filter: null, "
        "bounds: {a: [['', {}, true, false], [/2/, /2/, true, true]]}}}}}");
}
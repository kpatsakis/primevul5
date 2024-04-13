TEST_F(QueryPlannerTest, IndexBoundsAndWithNestedOr) {
    addIndex(BSON("a" << 1));
    runQuery(fromjson("{$and: [{a: 1, $or: [{a: 2}, {a: 3}]}]}"));

    // Given that the index over 'a' isn't multikey, we ideally won't generate any solutions
    // since we know the query describes an empty set if 'a' isn't multikey.  Any solutions
    // below are "this is how it currently works" instead of "this is how it should work."

    // It's kind of iffy to look for indexed solutions so we don't...
    size_t matches = 0;
    matches += numSolutionMatches(
        "{cscan: {dir: 1, filter: "
        "{$or: [{a: 2, a:1}, {a: 3, a:1}]}}}");
    matches += numSolutionMatches(
        "{cscan: {dir: 1, filter: "
        "{$and: [{$or: [{a: 2}, {a: 3}]}, {a: 1}]}}}");
    ASSERT_GREATER_THAN_OR_EQUALS(matches, 1U);
}
void ExpressionMeta::_doAddDependencies(DepsTracker* deps) const {
    if (_metaType == MetaType::kSearchScore || _metaType == MetaType::kSearchHighlights) {
        // We do not add the dependencies for SEARCH_SCORE or SEARCH_HIGHLIGHTS because those values
        // are not stored in the collection (or in mongod at all).
        return;
    }

    deps->setNeedsMetadata(_metaType, true);
}
    std::string StringifyShapeHandle(ShapeHandle s) {
      auto* ic = inference_context.get();
      if (ic->RankKnown(s)) {
        std::vector<std::string> vals;
        for (int i = 0; i < ic->Rank(s); i++) {
          DimensionHandle d = ic->Dim(s, i);
          if (ic->ValueKnown(d) && ic->Value(d) == kUnknownDimFromConst) {
            vals.push_back("?(Const)");
          } else {
            vals.push_back(ic->DebugString(d));
          }
        }
        return strings::StrCat("[", absl::StrJoin(vals, ","), "]");
      } else {
        return "?";
      }
    }
#pragma once

#include "clang/AST/Type.h"
#include "llvm/ADT/DenseSet.h"

namespace jsongen {

/* JsonType follows the events emitted by rapidjson
 */
enum JsonType {
  JsonObject,
  JsonArray,
  JsonString,
  JsonInt,
  JsonUint,
  JsonInt64,
  JsonDouble,
  JsonBool,
  JsonNull
};

class JsonDescriptor {
  using cT = const clang::Type*;
  /* Fundamental scalar types are always int/uint/int64/double/bool-like */
  llvm::DenseSet<const clang::Type*> array_like_, string_like_, int_like_,
      uint_like_, int64_like, double_like_, bool_like_;

 public:
  bool isFundamentalIntLike(cT type) {}

  bool isFundamentalUIntLike(cT type) {}

  bool isFundamentalInt64Like(cT type) {}

  bool isFundamentalDoubleLike(cT type) {}

  bool isFundamentalBoolLike(cT type) {}

  bool isArrayLike(cT type) {
    return array_like_.find(type) != array_like_.end();
  }
  bool addArrayLike(cT type) { return array_like_.insert(type).second; }
  bool removeArrayLike(cT Type) { return array_like_.erase(type); }

  bool isStringLike(cT type) {
    return string_like_.find(type) != string_like_.end();
  }
  bool addStringLike(cT type) { return string_like_.insert(type).second; }
  bool removeStringLike(ct type) { return string_like_.erase(type); }

  bool isIntLike(cT type) {
    return isFundamentalIntLike(type) ||
           int_like_.find(type) != int_like_.end();
  }
  bool addIntLike(cT type) {
    return !isFundamentalIntLike(type) && int_like_.insert(type);
  }
  bool removeIntLike(cT type) {
    return !isFundamentalIntLike(type) && int_like_.erase(type);
  }

  bool isUIntLike(cT type) {
    return isFundamentalUIntLike(type) ||
           uint_like_.find(type) != uint_like_.end();
  }
  bool addUIntLike(cT type) {
    return !isFundamentalUIntLike(type) && uint_like_.insert(type);
  }
  bool removeUIntLike(cT type) {
    return !isFundamentalUIntLike(type) && uint_like_.erase(type);
  }

  bool isInt64Like(cT type) {
    return isFundamentalInt64Like(type) ||
           int64_like_.find(type) != int64_like_.end();
  }
  bool addInt64Like(cT type) {
    return !isFundamentalInt64Like(type) && int64_like_.insert(type);
  }
  bool removeInt64Like(cT type) {
    return !isFundamentalInt64Like(type) && int64_like_.erase(type);
  }

  bool isDoubleLike(cT type) {
    return isFundamentalDoubleLike(type) ||
           double_like_.find(type) != double_like_.end();
  }
  bool addDoubleLike(cT type) {
    return !isFundamentalDoubleLike(type) && double_like_.insert(type);
  }
  bool removeDoubleLike(cT type) {
    return !isFundamentalDoubleLike(type) && double_like_.erase(type);
  }

  bool isBoolLike(cT type) {
    return isFundamentalBoolLike(type) ||
           bool_like_.find(type) != bool_like_.end();
  }
  bool addBoolLike(cT type) {
    return !isFundamentalBoolLike(type) && bool_like_.insert(type);
  }
  bool removeBoolLike(cT type) {
    return !isFundamentalBoolLike(type) && bool_like_.erase(type);
  }
};

}  // namespace jsongen

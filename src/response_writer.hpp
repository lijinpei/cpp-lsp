#pragma once
#include <system_error>
#include <string_view>

/* This file defines ResponseWriter class. All Concrete RequestReader should
 * simulate json-like data model, whether the underlay data-model is json,
 * protobuf or something else.
 */

namespace cpplsp {
template <typename RW, typename Obj>
struct ResponseWriterTrair;
/*
 * error_code writeObj(RR&&, Obj&&);
 */
template <typename DerivedRW>
class RequestReader {
  using std::pair;
  using EC = std::error_code;
public:
  EC writeInt(int);
  EC writeString(std::string_view);
  EC writeBool(bool);
  EC finish();
};
}

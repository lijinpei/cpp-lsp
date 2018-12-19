#pragma once
#include <system_error>

/* This file defines RequestReader class. All Concrete RequestReader should
 * simulate json-like data model, whether the underlay data-model is json,
 * protobuf or something else.
 */

namespace cpplsp {

template <typename RR, typename Obj>
struct RequestReaderTrait;
/*
 * error_code readObj(RR&&, Obj&&);
 */
template <typename DerivedRR>
class RequestReader {
  using std::pair;
  using EC = std::error_code;
public:
  pair<int, EC> readInt();
  pair<std::string, EC> readString();
  pair<bool, EC> readBool();
  pair<double, EC> readDouble();
  template <typename Obj>
  EC readObj(Obj && obj) {
    return RequestReaderTrait<RequestReader, Obj>::readObj(*this, obj);
  }
  EC finish();
};

}

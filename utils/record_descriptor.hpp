#pragma once

#include "json_descriptor.hpp"

#include "clang/AST/Type.h"
#include "llvm/Support/StringRef.h"
#include "llvm/ADT/DenseMap.h"


namespace jsongen {
  enum RecordDescriptorError {
    RDE_NoError,
    RDE_VirtualBase,
    RDE_ConflictingBaseFields
  };
  class RecordDescriptor;
  class RecordDescriptorRegistry {
    JsonDescriptor* jd_;
    // The Registry owns all the RecordDescriptors
    llvm::DenseMap<const clang::Type*, RecordDescriptor*> registry_;
  public:
    RecordDescriptorRegistry(JsonDescriptor& jd) : jd_(jd) {}
    std::pair<RecordDescriptor*, RecordDescriptorError> parse(const clang::Type* type) {
    }
    ~RecordDescriptorRegistry() {
      for (auto& kv : registry_) {
        delete kv.second;
        kv.second = nullptr;
      }
    }
  };
  class RecordDescriptor {
    RecordError re_;
    struct FieldDescriptor {
      JsonType type;
      union {
        RecordDescriptor* rec;
        const clang::Type* type;
      };
    };
    llvm::DenseMap<llvm::StringRef, FieldDescriptor> fields_;
    RecordDescriptor(JsonDescriptor& json_des) : re(RE_NoError), json_des_(json_des) {}
    friend class RecordDescriptorRegistry;
    RecordDescriptorError parse(const clang::Type* type, JsonDescriptor& jd, RecordDescriptorRegistry& reg) {
    }
  public:
  };
}

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"

#include "llvm/ADT/DenseSet.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/MemoryBuffer.h"
// Don't use llvm/CommandLine because it gets you lots of extra help messages
//#include "llvm/Support/CommandLine.h"
#include "llvm/Support/raw_ostream.h"

#include <cstring>
#include <regex>
#include <string>

class JsongenVisitor : public clang::RecursiveASTVisitor<JsongenVisitor> {
  clang::ASTContext& context_;
  llvm::raw_ostream& os_;
  std::vector<std::regex>& regex_;
  llvm::DenseSet<const clang::Type*> visited;
  /*
  std::vector<std::pair<std::string, clang::Type*>> getFields(clang::Type* type)
  {
  }
  bool emitCode(std::vector<std::pair<std::string, clang::Type*>> & fields) {
  }
  */

 public:
  JsongenVisitor(clang::ASTContext& context, llvm::raw_ostream& os,
                 std::vector<std::regex>& regex)
      : context_(context), os_(os), regex_(regex) {}
  bool VisitCXXRecordDecl(clang::CXXRecordDecl* decl) {
    if (!context_.getSourceManager().isInMainFile(decl->getLocation()) ||
        !decl->hasDefinition() ||
        !visited.insert(decl->getTypeForDecl()).second) {
      return true;
    }
    llvm::StringRef name = decl->getName();
    bool match = false;
    for (auto& reg : regex_) {
      if (std::regex_match(name.data(), reg)) {
        match = true;
        break;
      }
    }
    if (match) {
      llvm::outs() << name << '\n';
    }
    return true;
  }
};

class JsongenConsumer : public clang::ASTConsumer {
  clang::CompilerInstance& ci_;
  llvm::raw_ostream& os_;
  std::vector<std::regex>& regex_;

 public:
  JsongenConsumer(clang::CompilerInstance& ci, llvm::raw_ostream& os,
                  std::vector<std::regex>& regex)
      : ci_(ci), os_(os), regex_(regex) {}
  void HandleTranslationUnit(clang::ASTContext& context) override {
    JsongenVisitor visitor(context, os_, regex_);
    visitor.TraverseDecl(context.getTranslationUnitDecl());
  }
};

class JsongenAction : public clang::ASTFrontendAction {
  llvm::raw_ostream& os_;
  std::vector<std::regex>& regex_;

 public:
  JsongenAction(llvm::raw_ostream& os, std::vector<std::regex>& regex)
      : os_(os), regex_(regex) {}
  std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
      clang::CompilerInstance& ci, llvm::StringRef) {
    return llvm::make_unique<JsongenConsumer>(ci, os_, regex_);
  }
};

struct Config {
  int arg_start;
  llvm::StringRef of_name, emulation;
  std::vector<llvm::StringRef> if_names;
  std::vector<llvm::StringRef> regex;
};

void display_help_message() {}

bool parseCommandLine(Config& conf, int argc, char* argv[]) {
  bool has_output = false;
  bool has_emulation = false;
  conf.arg_start = argc;
  for (int i = 1; i < argc; ++i) {
    if (!strcmp(argv[i], "-o") || !strcmp(argv[i], "--output")) {
      if (++i == argc) {
        llvm::errs() << "expect output filename argument\n";
        return false;
      } else {
        if (has_output) {
          llvm::errs() << "can only specify one output file\n";
          return false;
        } else {
          has_output = true;
          conf.of_name = argv[i];
        }
      }
    } else if (!strcmp(argv[i], "-i") || !strcmp(argv[i], "--input")) {
      if (++i == argc) {
        llvm::errs() << "expect input file name argument\n";
        return false;
      }
      conf.if_names.emplace_back(argv[i]);
    } else if (!strcmp(argv[i], "-p") || !strcmp(argv[i], "--pattern")) {
      if (++i == argc) {
        llvm::errs() << "expect pattern argument\n";
        return false;
      }
      conf.regex.emplace_back(argv[i]);
    } else if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) {
      display_help_message();
    } else if (!strcmp(argv[i], "--")) {
      conf.arg_start = i + 1;
      break;
    } else if (!strcmp(argv[i], "-m") || !strcmp(argv[i], "--emulate")) {
      if (has_emulation) {
        llvm::errs() << "can only spcify one emulation\n";
        return false;
      }
      if (++i == argc) {
        llvm::errs() << "expect emulation argument\n";
        return false;
      }
      has_emulation = true;
      conf.emulation = argv[i];
    } else {
      llvm::errs() << "unknown argument: " << argv[i] << "\n";
      return false;
    }
  }
  if (!has_output) {
    llvm::errs() << "at least one output file required\n";
    return false;
  }
  if (!conf.if_names.size()) {
    llvm::errs() << "at least one input file required\n";
    return false;
  }
  if (!conf.regex.size()) {
    llvm::errs() << "at least one regex pattenr required\n";
    return false;
  }
  if (!has_emulation) {
    conf.emulation = "/usr/bin/clang++";
    has_emulation = true;
  }
  return true;
}

int main(int argc, char* argv[]) {
  Config conf;
  if (!parseCommandLine(conf, argc, argv)) {
    return 1;
  }
  llvm::SmallVector<char, 32> emulation;
  std::error_code ec;
  ec = llvm::sys::fs::real_path(conf.emulation, emulation, true);
  if (ec) {
    llvm::errs() << "unable to resolve emulation path: " << conf.emulation
                 << '\n';
    llvm::errs() << ec.message() << '\n';
    return 1;
  }
  llvm::raw_fd_ostream ofs(conf.of_name, ec,
                           llvm::sys::fs::CreationDisposition::CD_CreateAlways);
  if (ec) {
    llvm::outs() << "unable to open output file\n";
    return 1;
  }
  std::vector<std::regex> regex;
  for (auto& pat : conf.regex) {
    regex.emplace_back(pat.data());
  }
  std::vector<std::string> args;
  args.reserve(argc - conf.arg_start);
  for (int i = conf.arg_start; i < argc; ++i) {
    args.push_back(argv[i]);
  }
  for (auto& ifs : conf.if_names) {
    auto buf = llvm::MemoryBuffer::getFileAsStream(ifs);
    if (!buf) {
      llvm::errs() << "unable to open input file: " << ifs << '\n';
      return 0;
    }
    clang::tooling::runToolOnCodeWithArgs(new JsongenAction(ofs, regex),
                                          buf.get()->getBuffer(), args, ifs,
                                          emulation);
  }
}

#ifndef EMBEDDED_COVERAGE_CODE_INSERTER_H
#define EMBEDDED_COVERAGE_CODE_INSERTER_H

#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Format/Format.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Tooling/Core/Replacement.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/JSON.h"
#include "llvm/Support/raw_ostream.h"

#include <filesystem>
#include <unordered_set>

enum class Statements {
  None,
  If,
  Else,
  For,
  While,
  DoWhile,
  SwitchCase,
  Function,
  Lambda
};

static std::string StatementToString(const Statements statement);

/// Функция, проверяющая месторасположение файла обрабатываемого узла.
static bool isFileInWork(const std::string FileName);

static std::optional<clang::tooling::Replacement>
insertInclude(const clang::StringRef &PathFileToInsert,
              const clang::StringRef &PathIncludeFile);

static clang::format::FormatStyle GetFormatStyle();

static void formattingSourceCodeFile(const clang::StringRef &FileName);

/// Функция для рекурсивного обхода директории.
static void traverseDirectory(const std::filesystem::path &directory,
                              const std::vector<std::string> &extensions,
                              std::set<std::string> &fileSet);

/// Функция для получения значение из реестра.
static std::optional<std::wstring>
GetValueFromRegistry(const std::wstring &subKey, const std::wstring &key);

/// Функция для установки значения в реестр.
static bool SetValueInRegistry(const std::wstring &subKey,
                               const std::wstring &key,
                               const std::wstring &value);

/// Сборщик статистики.
class StatisticsCollector {
public:
  StatisticsCollector() = default;
  void add(const std::string &macroName);
  std::map<std::string, int> get() const;
  void print() const;

private:
  std::map<std::string, int> Statistics;
};

/// Класс, записывающий позиции всех расставленных макросов в файл json.
class JsonDataWriter {
public:
  JsonDataWriter(const std::string &outputFilename);
  ~JsonDataWriter();
  void write(const Statements statement, const int numberLine,
             const std::string &fileName, const int numberFileName);

private:
  bool isFileOpen = false;
  bool isFirstObject = true;
  std::string outputFilename;
  std::unique_ptr<llvm::raw_fd_ostream> outputFile;
};

/// Класс для обхода узлов построенного AST.
class StatementDeclarationVisitor
    : public clang::RecursiveASTVisitor<StatementDeclarationVisitor> {
public:
  explicit StatementDeclarationVisitor(clang::ASTContext *Context,
                                       clang::Rewriter &Rewrite);

  /// Шаблонный метод, описывающий единый способ работы с некоторыми узлами.
  template <typename TypeStmt>
  bool TemplateVisitStmt(const Statements Statement, TypeStmt *stmt) {
    clang::SourceManager &SM = Context->getSourceManager();
    clang::SourceLocation Location =
        stmt->getBody()->getBeginLoc().getLocWithOffset(1);
    std::string FileName = SM.getFilename(Location).str();
    if (!isFileInWork(FileName))
      return true;
    InsertMacros(Location, Statement);
    return true;
  }

  /// Метод вызываемый, при посещении узла for.
  bool VisitForStmt(clang::ForStmt *For);
  bool VisitCXXForRangeStmt(clang::CXXForRangeStmt *For);
  /// Метод вызываемый, при посещении узла while.
  bool VisitWhileStmt(clang::WhileStmt *While);
  /// Метод вызываемый, при посещении узла do while.
  bool VisitDoStmt(clang::DoStmt *Do);
  /// Метод вызываемый, при посещении узла лямбда функции.
  bool VisitLambdaExpr(clang::LambdaExpr *Lambda);
  /// Метод вызываемый, при посещении узла if
  bool VisitIfStmt(clang::IfStmt *If);
  /// Метод вызываемый, при посещении узла switch.
  bool VisitSwitchStmt(clang::SwitchStmt *Switch);
  /// Метод вызываемый, при посещении узла функции
  bool VisitFunctionDecl(clang::FunctionDecl *Func);
  /// Метод вызываемый, при посещении узла шаблонной функции.
  bool VisitFunctionTemplateDecl(clang::FunctionTemplateDecl *FuncTemplate);
  void InsertMacros(const clang::SourceLocation Location,
                    const Statements Statement);
  const StatisticsCollector& GetStatistics() const;

private:
  StatisticsCollector Statistics;
  JsonDataWriter JsonWriter;
  clang::ASTContext *Context;
  clang::Rewriter &Rewrite;
};

/// Класс-связка между построителем и потребителем AST.
class CoverageConsumer : public clang::ASTConsumer {
public:
  explicit CoverageConsumer(clang::ASTContext *Context);
  void HandleTranslationUnit(clang::ASTContext &Context) override;

private:
  StatementDeclarationVisitor Visitor;
  clang::Rewriter Rewrite;
};

class ASTCoverageAction : public clang::ASTFrontendAction {
public:
  std::unique_ptr<clang::ASTConsumer>
  CreateASTConsumer(clang::CompilerInstance &CI,
                    clang::StringRef file) override;
};

class IgnoreDiagnosticConsumer : public clang::DiagnosticConsumer {
public:
  void HandleDiagnostic(clang::DiagnosticsEngine::Level DiagLevel,
                        const clang::Diagnostic &Info) override {}
};

int main(int argc, const char **argv);

#endif
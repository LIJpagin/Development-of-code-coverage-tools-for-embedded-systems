#include "EmbeddedCoverageCodeInserter.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Inclusions/HeaderAnalysis.h"
#include "clang/Tooling/Inclusions/HeaderIncludes.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Signals.h"

#include <cstdlib>
#include <iostream>
#include <limits>
#include <numeric>
#include <windows.h>


// --include-file="D:\Work\A813-0409\TUKN00601\82.01\software\mfd_fw\src\soft\utils\loggers\CoverageLogger.h" D:\Work\A813-0409\TUKN00601\82.01\software\mfd_fw\src\soft\utils\testing\diskflash.cpp
using namespace clang;

static llvm::cl::OptionCategory
    EmbeddedCoverageCode("Embedded coverage code inserter");

/// Флаг командной строки для рекурсивного обхода директории.
static llvm::cl::opt<bool> RecursiveDirectoryTraversal(
    "recursive",
    llvm::cl::desc(
        R"(The presence of the flag indicates that one of the command arguments
        is passed a directory that must be traversed recursively by
        processing files with the specified extensions --extensions. Used
        together with --extensions.)"),
    llvm::cl::init(false), llvm::cl::cat(EmbeddedCoverageCode));

/// Флаг вывода имен, используемых для вставки макросов.
static llvm::cl::opt<bool> ShowMacrosName(
    "show-macros-name",
    llvm::cl::desc("Show macros to insert into the source code."),
    llvm::cl::init(false), llvm::cl::cat(EmbeddedCoverageCode));

/// Список расширений файлов, которые необходимо обработать.
static llvm::cl::list<std::string> Extensions(
    "extensions",
    llvm::cl::desc(R"(A set of strings with file extensions that need to be
                    processed when recursively traversing a directory. Used
                    in conjunction with --recursive.)"),
    llvm::cl::cat(EmbeddedCoverageCode));

/// Список конкретных файлов из директории, которые нужно исключить из
/// обработки.
static llvm::cl::list<std::string> ExcludeFiles(
    "exclude-files",
    llvm::cl::desc(R"(Allows you to specify files that need to be excluded when
         recursively traversing the directory. Used together with
         --recursive.)"),
    llvm::cl::cat(EmbeddedCoverageCode));

/// Список файлов для обработки, если не используется обход директории.
static llvm::cl::list<std::string>
    FileNames(llvm::cl::Positional, llvm::cl::desc("[@<file>] [<file> ...]"),
              llvm::cl::cat(EmbeddedCoverageCode));

/// Путь к подключаемому заголовочному файлу с логгером покрытия кода.
static llvm::cl::opt<std::string> IncludeFile(
    "include-file",
    llvm::cl::desc(
        "The path to the attached header file with the code coverage logger."),
    llvm::cl::init(""), llvm::cl::cat(EmbeddedCoverageCode));
static std::string IncludeFileSave("");

/// Путь к файлу json, в котором будет находится информация о расставленных
/// макросах.
static llvm::cl::opt<std::string> OutputFile(
    "output-file",
    llvm::cl::desc("The path to the json file, which will contain information "
                   "about the placed macros."),
    llvm::cl::init(""), llvm::cl::cat(EmbeddedCoverageCode));
static std::string OutputFileSave("");

static const std::wstring SubKeyFileNameCounter =
    L"Software\\EmbeddedCoverageCodeInserter";
static const std::wstring KeyFileNameCounter = L"COVERAGE_CODE_FILE_COUNTER";

static std::set<std::string> SourceFilesSet;

/// Ассоциативный массив операторов языка и макросов для вставки.
static std::map<Statements, std::string> MacrosName{
    {Statements::If, "COV_LOG_IF"},
    {Statements::Else, "COV_LOG_ELSE"},
    {Statements::For, "COV_LOG_FOR"},
    {Statements::While, "COV_LOG_WHILE"},
    {Statements::DoWhile, "COV_LOG_DO"},
    {Statements::SwitchCase, "COV_LOG_CASE"},
    {Statements::Function, "COV_LOG_FUNC"},
    {Statements::Lambda, "COV_LOG_LAMBDA"},
};

static std::string FileInWork = "";

static uint16_t FileNameCounter = 0;

static std::string StatementToString(const Statements statement) {
  switch (statement) {
  case Statements::If:
    return "if";
  case Statements::Else:
    return "else";
  case Statements::For:
    return "for";
  case Statements::While:
    return "while";
  case Statements::DoWhile:
    return "do while";
  case Statements::SwitchCase:
    return "switch case";
  case Statements::Function:
    return "function";
  case Statements::Lambda:
    return "lambda";
  case Statements::None:
  default:
    return "unknow";
  }
}

/// Функция, проверяющая месторасположение файла обрабатываемого узла
static bool isFileInWork(const std::string FileName) {
  return FileInWork == FileName;
}

static clang::format::FormatStyle GetFormatStyle() {
  using namespace clang::format;
  FormatStyle Style = getGoogleStyle(FormatStyle::LanguageKind::LK_Cpp);
  Style.AllowShortBlocksOnASingleLine = FormatStyle::ShortBlockStyle::SBS_Never;
  Style.AllowShortFunctionsOnASingleLine =
      FormatStyle::ShortFunctionStyle::SFS_None;
  Style.AllowShortIfStatementsOnASingleLine =
      FormatStyle::ShortIfStyle::SIS_Never;
  Style.AllowShortLambdasOnASingleLine =
      FormatStyle::ShortLambdaStyle::SLS_None;
  Style.AllowShortLoopsOnASingleLine = false;
  Style.BraceWrapping.AfterCaseLabel = true;
  Style.BraceWrapping.AfterControlStatement = FormatStyle::BWACS_Always;
  Style.BraceWrapping.AfterFunction = true;
  Style.BraceWrapping.BeforeElse = true;
  Style.BraceWrapping.BeforeLambdaBody = true;
  Style.BraceWrapping.BeforeWhile = false;
  Style.BraceWrapping.SplitEmptyFunction = true;
  Style.InsertBraces = true;
  Style.RemoveBracesLLVM = false;
  Style.SeparateDefinitionBlocks = FormatStyle::SDS_Always;
  Style.SortIncludes = FormatStyle::SI_Never;
  Style.IndentWidth = 4;
  Style.ColumnLimit = 0;
  return Style;
}

static void formattingSourceCodeFile(const StringRef &FileName) {
  using namespace clang::tooling;
  using namespace llvm;
  // Проверка имени файла
  if (FileName.empty()) {
    errs() << "Error: Empty file names are not allowed.\n";
    return;
  }

  // Чтение содержимого файла
  ErrorOr<std::unique_ptr<MemoryBuffer>> CodeOrErr =
      MemoryBuffer::getFile(FileName);

  // Обработка ошибок
  if (std::error_code EC = CodeOrErr.getError()) {
    errs() << EC.message() << "\n";
    return;
  }

  // Получение буфера кода
  std::unique_ptr<MemoryBuffer> Code = std::move(CodeOrErr.get());

  // Пустой файл
  if (Code->getBufferSize() == 0)
    return;

  // Получение строки из буфера кода
  StringRef BufStr = Code->getBuffer();

  // Проверка на поддержку спецификаций кодировки
  const char *InvalidBOM = SrcMgr::ContentCache::getInvalidBOM(BufStr);
  if (InvalidBOM) {
    errs() << "Error: Encoding with unsupported byte order sign found \""
           << InvalidBOM << "\" in file '" << FileName << "'"
           << ".\n";
    return;
  }

  std::vector<Range> Ranges;
  Ranges.push_back(Range(0, Code->getBuffer().size()));

  // Настройка стиля форматирования
  clang::format::FormatStyle Style = GetFormatStyle();

  // Форматирование
  clang::format::FormattingAttemptStatus Status;
  Replacements Replaces =
      reformat(Style, Code->getBuffer(), Ranges, FileName, &Status);

  // Отключаем диагностические сообщения
  IntrusiveRefCntPtr<DiagnosticOptions> DiagOpts(new DiagnosticOptions());
  IgnoreDiagnosticConsumer IgnoreDiagnostics;
  DiagnosticsEngine Diagnostics(
      IntrusiveRefCntPtr<DiagnosticIDs>(new DiagnosticIDs), &*DiagOpts,
      &IgnoreDiagnostics, false);

  // Создаем виртуальную файловую систему
  IntrusiveRefCntPtr<vfs::InMemoryFileSystem> InMemoryFileSystem(
      new vfs::InMemoryFileSystem);
  FileManager Files(FileSystemOptions(), InMemoryFileSystem);
  SourceManager Sources(Diagnostics, Files);

  InMemoryFileSystem.get()->addFileNoOwn(FileName, 0, *Code);
  auto File = Files.getOptionalFileRef(FileName);
  assert(File && "The file has not been added to MemFS");
  FileID ID = Sources.createFileID(*File, SourceLocation(), SrcMgr::C_User);

  Rewriter Rewrite(Sources, LangOptions());
  applyAllReplacements(Replaces, Rewrite);

  if (Rewrite.overwriteChangedFiles())
    return;
}

// Функция для рекурсивного обхода директории
static void traverseDirectory(const std::filesystem::path &directory,
                              const std::vector<std::string> &extensions,
                              std::set<std::string> &fileSet) {
  using namespace std::filesystem;
  for (const auto &entry : directory_iterator(directory)) {
    if (is_directory(entry.status())) {
      // Рекурсивно вызываем функцию для каждой поддиректории
      traverseDirectory(entry.path(), extensions, fileSet);
    } else if (is_regular_file(entry.status())) {
      // Проверяем, соответствует ли расширение файлу
      for (const auto &ext : extensions) {
        if (entry.path().extension() == ext) {
          fileSet.insert(entry.path().string());
          break; // Выходим из цикла, чтобы не добавлять файл дважды
        }
      }
    }
  }
}

static std::optional<std::wstring>
GetValueFromRegistry(const std::wstring &subKey, const std::wstring &key) {
  HKEY hKey;
  wchar_t buffer[255] = L"";
  DWORD dataSize = sizeof(buffer);
  if (RegOpenKeyEx(HKEY_CURRENT_USER, subKey.c_str(), 0, KEY_READ, &hKey) !=
      ERROR_SUCCESS)
    return std::nullopt;
  if (RegQueryValueEx(hKey, key.c_str(), NULL, NULL, (LPBYTE)buffer,
                      &dataSize) != ERROR_SUCCESS)
    return std::nullopt;
  if (RegCloseKey(hKey) != ERROR_SUCCESS)
    return std::nullopt;
  if (std::wcslen(buffer) == 0)
    return std::nullopt;
  else
    return std::wstring(buffer);
}

static bool SetValueInRegistry(const std::wstring &subKey,
                               const std::wstring &key,
                               const std::wstring &value) {
  HKEY hKey;
  LPCWSTR buffer = value.c_str();

  if (RegCreateKeyEx(HKEY_CURRENT_USER, subKey.c_str(), 0, NULL,
                     REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey,
                     NULL) != ERROR_SUCCESS)
    return false;
  if (RegSetValueEx(hKey, key.c_str(), 0, REG_SZ, (const BYTE *)buffer,
                    (wcslen(buffer) + 1) * sizeof(wchar_t)) != ERROR_SUCCESS)
    return false;
  if (RegCloseKey(hKey) != ERROR_SUCCESS)
    return false;
  return false;
}

static std::optional<clang::tooling::Replacement>
insertInclude(const StringRef &PathFileToInsert,
              const StringRef &PathIncludeFile) {
  using namespace clang::tooling;
  // Чтение содержимого файла
  llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>> CodeOrErr =
      llvm::MemoryBuffer::getFile(PathFileToInsert);

  // Обработка ошибок
  if (std::error_code EC = CodeOrErr.getError()) {
    llvm::errs() << EC.message() << "\n";
    return std::optional<Replacement>();
  }

  // Получение буфера кода
  std::unique_ptr<llvm::MemoryBuffer> Code = std::move(CodeOrErr.get());

  // Пустой файл
  if (Code->getBufferSize() == 0)
    return std::optional<Replacement>();
  StringRef BufStr = Code->getBuffer();

  // Получаем стандарт гугл формата и их формат includ'ов
  clang::format::FormatStyle GoogleStyle = clang::format::getGoogleStyle(
      clang::format::FormatStyle::LanguageKind::LK_Cpp);
  IncludeStyle &GoogleIncludeStyle = GoogleStyle.IncludeStyle;

  // Создаем класс управления включениями файлов
  HeaderIncludes HeaderInclusions(PathFileToInsert, BufStr, GoogleIncludeStyle);
  // Вставляем заголовочник
  return HeaderInclusions.insert(PathIncludeFile, false,
                                 IncludeDirective::Include);
}

void StatisticsCollector::add(const std::string &macroName) {
  Statistics[macroName]++;
}

std::map<std::string, int> StatisticsCollector::get() const {
  return Statistics;
}

void StatisticsCollector::print() const {
  if (!Statistics.empty())
    for (auto const &[macros, count] : Statistics)
      std::cout << macros << " - " << count << std::endl;
}

JsonDataWriter::JsonDataWriter(const std::string &outputFilename)
    : outputFilename(outputFilename) {
  std::error_code EC;
  outputFile = std::make_unique<llvm::raw_fd_ostream>(
      outputFilename, EC, llvm::sys::fs::OF_Text | llvm::sys::fs::OF_Append);
  if (EC) {
    llvm::errs() << "Error: Unable to open file " << outputFilename
                 << " for writing.\n";
    isFileOpen = false;
  }
  isFileOpen = true;
  *outputFile << "{\n\"Result\" : [\n";
}

JsonDataWriter::~JsonDataWriter() {
  if (isFileOpen) {
    *outputFile << "\n]\n}";
    outputFile->close();
  }
}

void JsonDataWriter::write(const Statements statement, const int numberLine,
                           const std::string &fileName,
                           const int numberFileName) {
  if (!isFileOpen)
    return;
  if (!isFirstObject)
    *outputFile << ",\n";
  else
    isFirstObject = false;
  llvm::json::OStream jsonStream(*outputFile);
  jsonStream.object([&]() {
    jsonStream.attribute("Statement", static_cast<int>(statement));
    jsonStream.attribute("NumberLine", numberLine);
    jsonStream.attribute("FileName",
                         std::filesystem::path(fileName).filename().string());
    jsonStream.attribute("NumberFileName", numberFileName);
  });
}

StatementDeclarationVisitor::StatementDeclarationVisitor(
    ASTContext *Context, Rewriter &Rewrite)
    : Context(Context), Rewrite(Rewrite), JsonWriter(OutputFileSave) {}

bool StatementDeclarationVisitor::VisitForStmt(ForStmt *For) {
  return TemplateVisitStmt(Statements::For, For);
}

bool StatementDeclarationVisitor::VisitCXXForRangeStmt(CXXForRangeStmt *For) {
  return TemplateVisitStmt(Statements::For, For);
}

bool StatementDeclarationVisitor::VisitWhileStmt(WhileStmt *While) {
  return TemplateVisitStmt(Statements::While, While);
}

bool StatementDeclarationVisitor::VisitDoStmt(DoStmt *Do) {
  return TemplateVisitStmt(Statements::DoWhile, Do);
}

bool StatementDeclarationVisitor::VisitLambdaExpr(LambdaExpr *Lambda) {
  return TemplateVisitStmt(Statements::Lambda, Lambda);
}

bool StatementDeclarationVisitor::VisitIfStmt(IfStmt *If) {
  SourceLocation IfLoc = If->getThen()->getBeginLoc().getLocWithOffset(1);
  std::string FileName = Context->getSourceManager().getFilename(IfLoc).str();
  if (!isFileInWork(FileName))
    return true;
  InsertMacros(IfLoc, Statements::If);
  if (If->hasElseStorage()) {
    if (const auto *Else = dyn_cast_or_null<IfStmt>(If->getElse())) {
      // Если нет блока else
      return true;
    } else {
      SourceLocation ElseLoc = If->getElse()->getBeginLoc().getLocWithOffset(1);
      InsertMacros(ElseLoc, Statements::Else);
    }
  }
  return true;
}

bool StatementDeclarationVisitor::VisitSwitchStmt(SwitchStmt *Switch) {
  SwitchCase *CurrentCase = Switch->getSwitchCaseList();
  while (CurrentCase) {
    SourceLocation CaseLoc = CurrentCase->getSubStmt()->getBeginLoc();
    std::string FileName =
        Context->getSourceManager().getFilename(CaseLoc).str();
    if (!isFileInWork(FileName))
      return true;
    InsertMacros(CaseLoc, Statements::SwitchCase);
    CurrentCase = CurrentCase->getNextSwitchCase();
  }
  return true;
}

bool StatementDeclarationVisitor::VisitFunctionDecl(FunctionDecl *Func) {
  if (!Func->isThisDeclarationADefinition())
    return true;
  if (Func->getBody()) {
    SourceLocation FuncLoc = Func->getBody()->getBeginLoc().getLocWithOffset(1);
    std::string FileName =
        Context->getSourceManager().getFilename(FuncLoc).str();
    if (!isFileInWork(FileName))
      return true;
    InsertMacros(FuncLoc, Statements::Function);
  }
  return true;
}

bool StatementDeclarationVisitor::VisitFunctionTemplateDecl(
    FunctionTemplateDecl *FuncTemplate) {
  FunctionDecl *FuncDecl = FuncTemplate->getTemplatedDecl();
  if (FuncDecl->isThisDeclarationADefinition()) {
    std::string FileName =
        Context->getSourceManager().getFilename(FuncDecl->getBeginLoc()).str();
    if (!isFileInWork(FileName))
      return true;
    if (Stmt *FuncBody = FuncDecl->getBody()) {
      SourceLocation FuncLoc = FuncBody->getBeginLoc().getLocWithOffset(3);
      InsertMacros(FuncLoc, Statements::Function);
    } else {
      llvm::errs() << "Warning: Template function "
                   << FuncDecl->getDeclName().getAsString() << " in file "
                   << FuncDecl->getLocation().printToString(
                          Context->getSourceManager())
                   << " not instantiated.\n";
      return true;
    }
  }
  return true;
}

void StatementDeclarationVisitor::InsertMacros(const SourceLocation Location,
                                               const Statements Statement) {
  std::string macros =
      MacrosName[Statement] + "(" + std::to_string(FileNameCounter) + ");";
  if (Rewrite.InsertText(Location, macros, true, true) == false) {
    Statistics.add(MacrosName[Statement]);
    SourceManager &SM = Context->getSourceManager();
    std::string FileName = SM.getFilename(Location).str();
    int LineNumber = SM.getSpellingLineNumber(Location);
    JsonWriter.write(Statement, LineNumber, FileName, FileNameCounter);
  }
}

const StatisticsCollector &StatementDeclarationVisitor::GetStatistics() const {
  return Statistics;
}

CoverageConsumer::CoverageConsumer(ASTContext *Context)
    : Rewrite(Context->getSourceManager(), Context->getLangOpts()),
      Visitor(Context, Rewrite) {}

void CoverageConsumer::HandleTranslationUnit(ASTContext &Context) {
  // Получаем имя файла
  SourceManager &SM = Context.getSourceManager();
  FileID MainFileID = SM.getMainFileID();
  StringRef FileName = SM.getFilename(SM.getLocForStartOfFile(MainFileID));
  // Вставляем заголовочник
  if (!IncludeFileSave.empty()) {
    auto OptionalReplace = insertInclude(FileName, IncludeFileSave);
    // Проверяем, что функция вернула значение и фиксируем изменения
    if (OptionalReplace.has_value())
      clang::tooling::applyAllReplacements(
          clang::tooling::Replacements(OptionalReplace.value()), Rewrite);
  }
  // Строим AST
  Visitor.TraverseDecl(Context.getTranslationUnitDecl());
  // Записываем все изменения
  Rewrite.overwriteChangedFiles();
  // Отрисовываем статистику вставок
  Visitor.GetStatistics().print();
  // Если хоть что-то вставили, увеличиваем счетчик имен файлов
  if (!Visitor.GetStatistics().get().empty())
    FileNameCounter = (FileNameCounter + 1) % UINT16_MAX;
}

std::unique_ptr<ASTConsumer>
ASTCoverageAction::CreateASTConsumer(CompilerInstance &CI, StringRef file) {
  CI.getDiagnostics().setClient(new IgnoringDiagConsumer, true);
  return std::make_unique<CoverageConsumer>(&CI.getASTContext());
}

int main(int argc, const char **argv) {
  llvm::sys::PrintStackTraceOnErrorSignal(argv[0]);
  // Парсим командную строку с флагами и настройками
  llvm::cl::ParseCommandLineOptions(argc, argv);

  // Получаем значение номера имени файла из реестра
  auto valueOpt =
      GetValueFromRegistry(SubKeyFileNameCounter, KeyFileNameCounter);
  if (valueOpt.has_value())
    FileNameCounter = std::stoi(valueOpt.value());
  else {
    llvm::errs()
        << "Error: The value could not be loaded from the registry. The "
           "value 0 will be applied.\n";
    FileNameCounter = 0;
  }

  // Сохраняем, иначе CommonOptionsParser их скушоет
  IncludeFileSave = IncludeFile.getValue();

  if (ShowMacrosName) {
    for (const auto &[key, value] : MacrosName)
      std::cout << StatementToString(key) << " has value " << value
                << std::endl;
    return 0;
  }

  // Добавляем файлы из директории
  if (RecursiveDirectoryTraversal == true && !FileNames.empty()) {
    if (Extensions.empty()) {
      llvm::errs() << "Error: File extensions are not specified.\n";
      return 1;
    }
    std::set<std::string> files;
    // Если происходит рекурсивный обход директории, пользователь обязан
    // 1 аргументом после флагов и настроек передать имя директории
    std::filesystem::path directoryPath = FileNames.front();
    if (OutputFile.empty())
      OutputFileSave =
          (std::filesystem::path(FileNames.front()).parent_path() /
           std::filesystem::path("CoverageCodeInserterResult.json"))
              .string();
    FileNames.erase(FileNames.begin());
    // Если после извлечения имени директории, список не пуст, значит
    // оставшиеся аргументы - имена файлов для обработки
    if (!FileNames.empty())
      SourceFilesSet.insert(FileNames.begin(), FileNames.end());
    // Проверяем существует ли директория согласно имени и директория ли это
    if (!std::filesystem::exists(directoryPath) ||
        !std::filesystem::is_directory(directoryPath)) {
      llvm::errs() << "Error: Invalid directory path.\n";
      return 1;
    }
    // Производим рекурсивный обход директории и собираем вложенные файлы
    // с указанным расширением
    traverseDirectory(directoryPath, Extensions, files);
    // Преобразуем файлы-исключения в сет для более простого поиска
    std::set<std::string> ExcludeFilesSet(ExcludeFiles.begin(),
                                          ExcludeFiles.end());
    // Пробегаемся по всем файлам и добавляем их в сет исходных файлов,
    // проверяя не являются ли они файлами-исключениями
    for (auto fileIt = files.begin(); fileIt != files.end(); ++fileIt) {
      if (ExcludeFilesSet.find(*fileIt) == ExcludeFilesSet.end())
        SourceFilesSet.insert(*fileIt);
    }
  } else {
    // Если пользователь не передал параметр рекурсивного обхода,
    // просто считаем, что он передает исходные файлы каждым аргументом
    SourceFilesSet.insert(FileNames.begin(), FileNames.end());
    if (OutputFile.empty())
      OutputFileSave =
          (std::filesystem::path(FileNames.front()).parent_path() /
           std::filesystem::path("CoverageCodeInserterResult.json"))
              .string();
  }

  // Собираем новые аргументы с учетом распарсенных настроек
  // из путей к файлам для парсера
  std::vector<const char *> args;
  args.reserve(1 + SourceFilesSet.size());
  // Обязательно добавим имя исполняемого файла
  args.push_back(argv[0]);
  std::transform(SourceFilesSet.begin(), SourceFilesSet.end(),
                 std::back_inserter(args),
                 [](const std::string &file) { return file.c_str(); });
  int countArgs = args.size();

  // Создаем парсер и отдаем ему файлы для обработки
  auto ExpectedParser = clang::tooling::CommonOptionsParser::create(
      countArgs, args.data(), EmbeddedCoverageCode);
  if (!ExpectedParser) {
    llvm::errs() << ExpectedParser.takeError();
    return 1;
  }
  auto &OptionsParser = ExpectedParser.get();

  unsigned FileNo = 1;
  for (const auto &FileName : SourceFilesSet) {
    std::cout << "[" << FileNo++ << "/" << SourceFilesSet.size() << "] file "
              << FileName << std::endl;
    FileInWork = FileName;
    formattingSourceCodeFile(FileName);
    // Запускаем наш инструментарии, который построит AST и произведет вставки
    clang::tooling::ClangTool Tool(OptionsParser.getCompilations(),
                                   std::vector({FileName}));
    int Result = Tool.run(
        clang::tooling::newFrontendActionFactory<ASTCoverageAction>().get());
    if (Result != 0)
      llvm::errs() << "An error in the operation of the toolkit.\n";
  }

  // Сохраняем значение номера имени файла в реестре
  if (SetValueInRegistry(SubKeyFileNameCounter, KeyFileNameCounter,
                         std::to_wstring(FileNameCounter)) == false)
    llvm::errs() << "Error: The value could not be saved in the registry.\n";

  return 0;
}

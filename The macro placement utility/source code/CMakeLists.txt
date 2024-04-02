set( LLVM_LINK_COMPONENTS
  ${LLVM_TARGETS_TO_BUILD}
  Option
  Support
  )

add_clang_tool(embedded-coverage-code-inserter
  EmbeddedCoverageCodeInserter.cpp
  )

clang_target_link_libraries(embedded-coverage-code-inserter
  PRIVATE
  clangAST
  clangBasic
  clangFrontend
  clangTooling
  )

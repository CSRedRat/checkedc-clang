//===--- Refactoring.h - Framework for clang refactoring tools --*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//  Interfaces supporting refactorings that span multiple translation units.
//  While single translation unit refactorings are supported via the Rewriter,
//  when refactoring multiple translation units changes must be stored in a
//  SourceManager independent form, duplicate changes need to be removed, and
//  all changes must be applied at once at the end of the refactoring so that
//  the code is always parseable.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_TOOLING_REFACTORING_H
#define LLVM_CLANG_TOOLING_REFACTORING_H

#include "clang/Tooling/Core/Replacement.h"
#include "clang/Tooling/Tooling.h"
#include <string>

namespace clang {

class Rewriter;

namespace tooling {

/// \brief A tool to run refactorings.
///
/// This is a refactoring specific version of \see ClangTool. FrontendActions
/// passed to run() and runAndSave() should add replacements to
/// getReplacements().
class RefactoringTool : public ClangTool {
public:
  /// \see ClangTool::ClangTool.
  RefactoringTool(const CompilationDatabase &Compilations,
                  ArrayRef<std::string> SourcePaths,
                  std::shared_ptr<PCHContainerOperations> PCHContainerOps =
                      std::make_shared<PCHContainerOperations>());

  /// \brief Returns the set of replacements to which replacements should
  /// be added during the run of the tool.
  Replacements &getReplacements();

  /// \brief Call run(), apply all generated replacements, and immediately save
  /// the results to disk.
  ///
  /// \returns 0 upon success. Non-zero upon failure.
  int runAndSave(FrontendActionFactory *ActionFactory);

  /// \brief Apply all stored replacements to the given Rewriter.
  ///
  /// Replacement applications happen independently of the success of other
  /// applications.
  ///
  /// \returns true if all replacements apply. false otherwise.
  bool applyAllReplacements(Rewriter &Rewrite);

private:
  /// \brief Write all refactored files to disk.
  int saveRewrittenFiles(Rewriter &Rewrite);

private:
  Replacements Replace;
};

/// \brief Groups \p Replaces by the file path and applies each group of
/// Replacements on the related file in \p Rewriter. In addition to applying
/// given Replacements, this function also formats the changed code.
///
/// \pre Replacements must be conflict-free.
///
/// Replacement applications happen independently of the success of other
/// applications.
///
/// \param[in] Replaces Replacements to apply.
/// \param[in] Rewrite The `Rewritter` to apply replacements on.
/// \param[in] Style The style name used for reformatting. See ```getStyle``` in
/// "include/clang/Format/Format.h" for all possible style forms.
///
/// \returns true if all replacements applied and formatted. false otherwise.
bool formatAndApplyAllReplacements(const Replacements &Replaces,
                                   Rewriter &Rewrite, StringRef Style = "file");

} // end namespace tooling
} // end namespace clang

#endif // LLVM_CLANG_TOOLING_REFACTORING_H

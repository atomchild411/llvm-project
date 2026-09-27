//===--- IRIX.h - IRIX ToolChain Implementations ----------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_IRIX_H
#define LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_IRIX_H

#include "clang/Driver/Tool.h"
#include "clang/Driver/ToolChain.h"

namespace clang {
namespace driver {
namespace toolchains {

/// SGI IRIX 6.5: n32 (the default) and n64, linked with lld for IRIX's own
/// runtime linker, rld, with compiler-rt, libunwind and libc++.
class LLVM_LIBRARY_VISIBILITY IRIX : public ToolChain {
public:
  IRIX(const Driver &D, const llvm::Triple &Triple,
       const llvm::opt::ArgList &Args);

  bool HasNativeLLVMSupport() const override { return true; }
  bool IsIntegratedAssemblerDefault() const override { return true; }
  // IsMathErrnoDefault stays true: IRIX's libm sets errno (EDOM for
  // sin(inf) or log(-1), ERANGE for exp(1000)), so its math functions may
  // not be treated as having no side effects unless asked (-fno-math-errno,
  // -ffast-math).
  RuntimeLibType GetDefaultRuntimeLibType() const override {
    return ToolChain::RLT_CompilerRT;
  }
  UnwindLibType GetDefaultUnwindLibType() const override {
    return ToolChain::UNW_CompilerRT;
  }
  CXXStdlibType GetDefaultCXXStdlibType() const override {
    return ToolChain::CST_Libcxx;
  }
  UnwindTableLevel
  getDefaultUnwindTableLevel(const llvm::opt::ArgList &Args) const override {
    return UnwindTableLevel::None;
  }
  bool isPICDefault() const override { return true; }
  bool isPIEDefault(const llvm::opt::ArgList &Args) const override {
    return false;
  }
  bool isPICDefaultForced() const override { return false; }
  // IRIX's dbx reads DWARF 2; tuning for dbx would drop the pubnames it
  // wants, so tune for gdb.
  llvm::DebuggerKind getDefaultDebuggerTuning() const override {
    return llvm::DebuggerKind::GDB;
  }
  unsigned GetDefaultDwarfVersion() const override { return 2; }

  void
  addClangTargetOptions(const llvm::opt::ArgList &DriverArgs,
                        llvm::opt::ArgStringList &CC1Args,
                        Action::OffloadKind DeviceOffloadKind) const override;
  void
  AddClangSystemIncludeArgs(const llvm::opt::ArgList &DriverArgs,
                            llvm::opt::ArgStringList &CC1Args) const override;
  void AddClangCXXStdlibIncludeArgs(
      const llvm::opt::ArgList &DriverArgs,
      llvm::opt::ArgStringList &CC1Args) const override;
  void AddCXXStdlibLibArgs(const llvm::opt::ArgList &Args,
                           llvm::opt::ArgStringList &CmdArgs) const override;

  const char *getDefaultLinker() const override { return "ld.lld"; }

  std::string getDynamicLinker(const llvm::opt::ArgList &Args) const;

  StringRef getABI() const { return ABI; }

protected:
  Tool *buildLinker() const override;

private:
  std::string ABI;
  // Where LLVM's libc++ was found beside the driver, if it was.
  std::string RuntimeLibDir;
  std::string LibSuffix;
};

} // end namespace toolchains

namespace tools {
namespace irix {

class LLVM_LIBRARY_VISIBILITY Linker final : public Tool {
public:
  Linker(const ToolChain &TC) : Tool("irix::Linker", "linker", TC) {}

  bool hasIntegratedCPP() const override { return false; }
  bool isLinkJob() const override { return true; }

  void ConstructJob(Compilation &C, const JobAction &JA,
                    const InputInfo &Output, const InputInfoList &Inputs,
                    const llvm::opt::ArgList &TCArgs,
                    const char *LinkingOutput) const override;
};

} // end namespace irix
} // end namespace tools

} // end namespace driver
} // end namespace clang

#endif // LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_IRIX_H

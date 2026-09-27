//===--- IRIX.cpp - IRIX ToolChain Implementations --------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// Based on Vladimir Vukicevic's LLVM 14 IRIX toolchain
// (github.com/vvuk/llvm-project, irix/14.x).
//
//===----------------------------------------------------------------------===//

#include "IRIX.h"
#include "Arch/Mips.h"
#include "clang/Config/config.h"
#include "clang/Driver/CommonArgs.h"
#include "clang/Driver/Compilation.h"
#include "clang/Driver/Driver.h"
#include "clang/Driver/Options.h"
#include "llvm/Option/ArgList.h"
#include "llvm/Support/Path.h"

using namespace clang::driver;
using namespace clang::driver::tools;
using namespace clang::driver::toolchains;
using namespace clang;
using namespace llvm::opt;

IRIX::IRIX(const Driver &D, const llvm::Triple &Triple, const ArgList &Args)
    : ToolChain(D, Triple, Args) {
  // Tools (ld.lld) next to the driver.
  getProgramPaths().push_back(std::string(D.Dir));

  StringRef CPU, ABIName;
  tools::mips::getMipsCPUAndABI(Args, Triple, CPU, ABIName);
  ABI = ABIName.str();
  // "32" for n32, "64" for n64: IRIX keeps each ABI's libraries in
  // /usr/lib32 and /usr/lib64, with ISA-specific ones in mips3/ and mips4/.
  LibSuffix = tools::mips::getMipsABILibSuffix(Args, Triple);

  const std::string &SysRoot = D.SysRoot;

  // SGUG-RSE, when present, is where most third-party libraries live.
  addPathIfExists(D, SysRoot + "/usr/sgug/lib" + LibSuffix, getFilePaths());
  // The ISA the code is built for, so that n32 code built for MIPS III links
  // against MIPS III libraries (and runs on an R4400) even on an R10000.
  getFilePaths().push_back(SysRoot + "/usr/lib" + LibSuffix + "/" + CPU.str());
  getFilePaths().push_back(SysRoot + "/usr/lib" + LibSuffix);
  getFilePaths().push_back(SysRoot + "/lib" + LibSuffix);
  // LLVM's own runtimes, if installed as a tree beside the driver.
  addPathIfExists(D, llvm::sys::path::parent_path(D.Dir) + "/lib" + LibSuffix,
                  getFilePaths());
}

Tool *IRIX::buildLinker() const { return new tools::irix::Linker(*this); }

std::string IRIX::getDynamicLinker(const ArgList &Args) const {
  // IRIX executables name libc as their interpreter; the kernel starts rld.
  return "/usr/lib" + LibSuffix + "/libc.so.1";
}

void IRIX::addClangTargetOptions(const ArgList &DriverArgs,
                                 ArgStringList &CC1Args,
                                 Action::OffloadKind) const {
  // rld runs DT_INIT/DT_FINI, not DT_INIT_ARRAY: constructors go in .ctors,
  // which compiler-rt's crtbegin walks.
  if (!DriverArgs.hasFlag(options::OPT_fuse_init_array,
                          options::OPT_fno_use_init_array, false))
    CC1Args.push_back("-fno-use-init-array");
}

void IRIX::AddClangSystemIncludeArgs(const ArgList &DriverArgs,
                                     ArgStringList &CC1Args) const {
  const Driver &D = getDriver();

  if (DriverArgs.hasArg(options::OPT_nostdinc))
    return;

  if (!DriverArgs.hasArg(options::OPT_nobuiltininc)) {
    addSystemInclude(DriverArgs, CC1Args, D.ResourceDir + "/include");
    // What IRIX's own headers lack (C99 format macros before 6.5.22, ...):
    // each wrapper includes IRIX's header and adds only what is missing.
    addSystemInclude(DriverArgs, CC1Args,
                     D.ResourceDir + "/include/irix_wrappers");
  }

  if (DriverArgs.hasArg(options::OPT_nostdlibinc))
    return;

  // Configure-time C include directories, if any, replace the defaults.
  StringRef CIncludeDirs(C_INCLUDE_DIRS);
  if (!CIncludeDirs.empty()) {
    SmallVector<StringRef, 5> Dirs;
    CIncludeDirs.split(Dirs, ":");
    for (StringRef Dir : Dirs) {
      StringRef Prefix =
          llvm::sys::path::is_absolute(Dir) ? "" : StringRef(D.SysRoot);
      addExternCSystemInclude(DriverArgs, CC1Args, Prefix + Dir);
    }
    return;
  }

  // SGUG-RSE's headers, when present.
  addSystemInclude(DriverArgs, CC1Args, D.SysRoot + "/usr/sgug/include");
  // Fixed copies of IRIX headers clang cannot take as they are, generated
  // from the target's own headers: in the IRIX tree, or beside the driver.
  addSystemInclude(DriverArgs, CC1Args,
                   D.SysRoot + "/usr/lib/clang/include-fixed");
  addSystemInclude(DriverArgs, CC1Args, D.ResourceDir + "/include-fixed");
  // IRIX's own headers.
  addSystemInclude(DriverArgs, CC1Args, D.SysRoot + "/usr/include");
}

void IRIX::AddClangCXXStdlibIncludeArgs(const ArgList &DriverArgs,
                                        ArgStringList &CC1Args) const {
  if (DriverArgs.hasArg(options::OPT_nostdlibinc) ||
      DriverArgs.hasArg(options::OPT_nostdincxx))
    return;

  switch (GetCXXStdlibType(DriverArgs)) {
  case ToolChain::CST_Libcxx: {
    SmallString<128> P(llvm::sys::path::parent_path(getDriver().Dir));
    llvm::sys::path::append(P, "include", getTripleString(), "c++", "v1");
    addSystemInclude(DriverArgs, CC1Args, P);

    P = llvm::sys::path::parent_path(getDriver().Dir);
    llvm::sys::path::append(P, "include", "c++", "v1");
    addSystemInclude(DriverArgs, CC1Args, P);
    break;
  }
  case ToolChain::CST_Libstdcxx:
    getDriver().Diag(diag::err_drv_unsupported_opt_for_target)
        << "-stdlib=libstdc++" << getTripleString();
    break;
  }
}

void IRIX::AddCXXStdlibLibArgs(const ArgList &Args,
                               ArgStringList &CmdArgs) const {
  switch (GetCXXStdlibType(Args)) {
  case ToolChain::CST_Libcxx:
    CmdArgs.push_back("-lc++");
    break;
  case ToolChain::CST_Libstdcxx:
    getDriver().Diag(diag::err_drv_unsupported_opt_for_target)
        << "-stdlib=libstdc++" << getTripleString();
    break;
  }
}

void irix::Linker::ConstructJob(Compilation &C, const JobAction &JA,
                                const InputInfo &Output,
                                const InputInfoList &Inputs,
                                const ArgList &Args,
                                const char *LinkingOutput) const {
  const auto &TC = static_cast<const toolchains::IRIX &>(getToolChain());
  const Driver &D = TC.getDriver();
  const bool IsShared = Args.hasArg(options::OPT_shared);
  const bool IsStatic = Args.hasArg(options::OPT_static);
  const bool StartFiles =
      !Args.hasArg(options::OPT_nostartfiles, options::OPT_nostdlib);

  ArgStringList CmdArgs;

  // Silence "argument unused" for clang -g/-emit-llvm/-w foo.o -o foo.
  Args.ClaimAllArgs(options::OPT_g_Group);
  Args.ClaimAllArgs(options::OPT_emit_llvm);
  Args.ClaimAllArgs(options::OPT_w);

  if (!D.SysRoot.empty())
    CmdArgs.push_back(Args.MakeArgString("--sysroot=" + D.SysRoot));

  if (!IsShared && Args.hasArg(options::OPT_pie))
    CmdArgs.push_back("-pie");

  // IRIX has no RELRO.
  CmdArgs.push_back("-z");
  CmdArgs.push_back("norelro");
  // rld (6.5.7 at least) reads DT_RPATH and ignores DT_RUNPATH.
  CmdArgs.push_back("--disable-new-dtags");
  // RPM's debuginfo tooling wants a build ID.
  CmdArgs.push_back("--build-id");
  CmdArgs.push_back("--eh-frame-hdr");

  if (IsStatic) {
    CmdArgs.push_back("-static");
  } else {
    if (Args.hasArg(options::OPT_rdynamic))
      CmdArgs.push_back("-export-dynamic");
    if (IsShared) {
      CmdArgs.push_back("-shared");
    } else {
      CmdArgs.push_back("-dynamic-linker");
      CmdArgs.push_back(Args.MakeArgString(TC.getDynamicLinker(Args)));
    }
  }

  CmdArgs.push_back("-m");
  CmdArgs.push_back(TC.getABI() == "n64" ? "elf64btsmip_irix"
                                         : "elf32btsmipn32_irix");

  if (Arg *A = Args.getLastArg(options::OPT_G)) {
    CmdArgs.push_back(Args.MakeArgString("-G" + StringRef(A->getValue())));
    A->claim();
  }

  assert((Output.isFilename() || Output.isNothing()) && "Invalid output.");
  if (Output.isFilename()) {
    CmdArgs.push_back("-o");
    CmdArgs.push_back(Output.getFilename());
  }

  if (StartFiles) {
    // IRIX's own crt1.o defines __start; executables only.
    if (!IsShared)
      CmdArgs.push_back(Args.MakeArgString(TC.GetFilePath("crt1.o")));
    // compiler-rt's crtbegin runs the .ctors list (and __cxa_finalize).
    CmdArgs.push_back(
        TC.getCompilerRTArgString(Args, "crtbegin", ToolChain::FT_Object));
  }

  Args.addAllArgs(CmdArgs, {options::OPT_L, options::OPT_u,
                            options::OPT_T_Group, options::OPT_e,
                            options::OPT_s, options::OPT_t,
                            options::OPT_Z_Flag, options::OPT_r});
  TC.AddFilePathLibArgs(Args, CmdArgs);

  if (D.isUsingLTO()) {
    assert(!Inputs.empty() && "Must have at least one input.");
    addLTOOptions(TC, Args, CmdArgs, Output, Inputs,
                  D.getLTOMode() == LTOK_Thin);
  }

  AddLinkerInputs(TC, Inputs, Args, CmdArgs, JA);

  bool LinkedRuntime = false;
  if (!Args.hasArg(options::OPT_nostdlib, options::OPT_nodefaultlibs)) {
    if (D.CCCIsCXX() && TC.ShouldLinkCXXStdlib(Args)) {
      bool OnlyLibcxxStatic = Args.hasArg(options::OPT_static_libstdcxx) &&
                              !IsStatic;
      CmdArgs.push_back("--push-state");
      CmdArgs.push_back("--as-needed");
      if (OnlyLibcxxStatic)
        CmdArgs.push_back("-Bstatic");
      TC.AddCXXStdlibLibArgs(Args, CmdArgs);
      if (OnlyLibcxxStatic)
        CmdArgs.push_back("-Bdynamic");
      CmdArgs.push_back("-lm");
      CmdArgs.push_back("-lpthread");
      CmdArgs.push_back("--pop-state");
      // Builtins and the unwinder.
      AddRunTimeLibs(TC, D, CmdArgs, Args);
      LinkedRuntime = true;
    }

    if (Args.hasArg(options::OPT_pthread))
      CmdArgs.push_back("-lpthread");

    if (!Args.hasArg(options::OPT_nolibc))
      CmdArgs.push_back("-lc");
  }

  // C links take only the builtins: AddRunTimeLibs would add the unwinder.
  if (!LinkedRuntime && !Args.hasArg(options::OPT_nostdlib))
    CmdArgs.push_back(
        TC.getCompilerRTArgString(Args, "builtins", ToolChain::FT_Static));

  if (StartFiles) {
    CmdArgs.push_back(
        TC.getCompilerRTArgString(Args, "crtend", ToolChain::FT_Object));
    if (!IsShared)
      CmdArgs.push_back(Args.MakeArgString(TC.GetFilePath("crtn.o")));
  }

  TC.addProfileRTLibs(Args, CmdArgs);

  const char *Exec = Args.MakeArgString(TC.GetLinkerPath());
  C.addCommand(std::make_unique<Command>(JA, *this,
                                         ResponseFileSupport::AtFileCurCP(),
                                         Exec, CmdArgs, Inputs, Output));
}

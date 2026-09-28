//===-- MipsTargetTransformInfo.cpp - Mips specific TTI ----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "MipsTargetTransformInfo.h"
#include "llvm/Support/CommandLine.h"

using namespace llvm;

extern cl::opt<bool> UseMipsTailCalls;

bool MipsTTIImpl::supportsTailCalls() const {
  return UseMipsTailCalls && !ST->inMips16Mode();
}

bool MipsTTIImpl::hasDivRemOp(Type *DataType, bool IsSigned) const {
  EVT VT = TLI->getValueType(DL, DataType);
  return TLI->isOperationLegalOrCustom(IsSigned ? ISD::SDIVREM : ISD::UDIVREM,
                                       VT);
}

// Tuned for the R10000, which issues out of order from a 32-entry window:
// unroll (LoopMicroOpBufferSize in its model) and interleave loops, so that
// independent work -- a reduction's partial sums, say -- overlaps. It has 32
// integer and 32 floating-point registers, and no vector ones.
unsigned MipsTTIImpl::getNumberOfRegisters(unsigned ClassID) const {
  if (!ST->isTunedForR10000())
    return BaseT::getNumberOfRegisters(ClassID);
  bool Vector = ClassID == 1;
  return Vector ? 0 : 32;
}

unsigned MipsTTIImpl::getMaxInterleaveFactor(ElementCount VF) const {
  if (!ST->isTunedForR10000())
    return BaseT::getMaxInterleaveFactor(VF);
  return 4;
}

bool MipsTTIImpl::isLSRCostLess(const TargetTransformInfo::LSRCost &C1,
                                const TargetTransformInfo::LSRCost &C2) const {
  // MIPS specific here are "instruction number 1st priority".
  // If we need to emit adds inside the loop to add up base registers, then
  // we need at least one extra temporary register.
  unsigned C1NumRegs = C1.NumRegs + (C1.NumBaseAdds != 0);
  unsigned C2NumRegs = C2.NumRegs + (C2.NumBaseAdds != 0);
  return std::tie(C1.Insns, C1NumRegs, C1.AddRecCost, C1.NumIVMuls,
                  C1.NumBaseAdds, C1.ScaleCost, C1.ImmCost, C1.SetupCost) <
         std::tie(C2.Insns, C2NumRegs, C2.AddRecCost, C2.NumIVMuls,
                  C2.NumBaseAdds, C2.ScaleCost, C2.ImmCost, C2.SetupCost);
}

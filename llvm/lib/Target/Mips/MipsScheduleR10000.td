//==- MipsScheduleR10000.td - R10000 Scheduling Definitions -*- tablegen -*-==//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// The MIPS R10000, and the R12000, R14000 and R16000 built on it: MIPS IV,
// four instructions decoded per cycle into three 16-entry queues (integer,
// address, floating point), issued out of order to five units, and retired
// in order from a 32-entry active list (48 from the R12000).
//
//   ALU1   add, subtract, logic; the shifter, branches, traps and
//          conditional moves
//   ALU2   add, subtract, logic; integer multiply and divide, which are
//          iterative and hold the multiply/divide unit for several cycles
//   LS     load and store address calculation, one per cycle
//   FPADD  add, subtract, compare, convert, move; pipelined
//   FPMUL  multiply, pipelined; divide, square root and the reciprocals go
//          through its issue port to an iterative divide/square-root unit
//
// Latencies and repeat rates are those the R10000 Microprocessor User's
// Manual tabulates. A multiply-add passes through the multiplier and then the
// adder, taking its addend there. Integer multiplies and divides give LO a
// cycle before HI; LO's latency is used.
//
// Instructions are mapped by itinerary class, which every MIPS instruction
// carries, so the model stays complete as instructions are added.
//
//===----------------------------------------------------------------------===//

def MipsR10000Model : SchedMachineModel {
  int IssueWidth = 4;
  int MicroOpBufferSize = 32; // the active list
  // What BasicTTIImpl unrolls loops to, partially or at run time.
  int LoopMicroOpBufferSize = 32;
  int LoadLatency = 2;
  int MispredictPenalty = 5;

  let CompleteModel = 1;

  // Nothing past MIPS IV.
  list<Predicate> UnsupportedFeatures = [HasMips5_32r2, HasMips32, HasMips32r2,
                                         HasMips32r5, HasMips32r6, HasMips64,
                                         HasMips64r2, HasMips64r5, HasMips64r6,
                                         InMicroMips, InMips16Mode,
                                         HasCnMips, HasCnMipsP,
                                         HasDSP, HasDSPR2, HasDSPR3, HasMips3D,
                                         HasMSA, HasMT, HasCRC, HasVirt,
                                         HasGINV, HasEVA];
}

let SchedModel = MipsR10000Model in {

// Queues and units
// ================

def R10KIntQueue : ProcResource<2> { let BufferSize = 16; }
def R10KALU1 : ProcResource<1> { let Super = R10KIntQueue; }
def R10KALU2 : ProcResource<1> { let Super = R10KIntQueue; }
def R10KALU : ProcResGroup<[R10KALU1, R10KALU2]>;
def R10KMulDiv : ProcResource<1>; // in ALU2

def R10KAddrQueue : ProcResource<1> { let BufferSize = 16; }
def R10KLS : ProcResource<1> { let Super = R10KAddrQueue; }

def R10KFPQueue : ProcResource<2> { let BufferSize = 16; }
def R10KFPAdd : ProcResource<1> { let Super = R10KFPQueue; }
def R10KFPMul : ProcResource<1> { let Super = R10KFPQueue; }
def R10KFPDivSqrt : ProcResource<1>; // behind the multiplier

// Integer
// =======

def R10KWriteALU : SchedWriteRes<[R10KALU]>;
def R10KWriteALU1 : SchedWriteRes<[R10KALU1]>;
def R10KWriteNop : SchedWriteRes<[]> { let Latency = 0; }

// Either ALU.
def : ItinRW<[R10KWriteALU], [II_ADD, II_ADDI, II_ADDIU, II_ADDU, II_AND,
                              II_ANDI, II_DADD, II_DADDI, II_DADDIU, II_DADDU,
                              II_DSUB, II_DSUBU, II_LI, II_LUI, II_MOVE, II_NOR,
                              II_NOT, II_OR, II_ORI, II_SLT_SLTU, II_SLTI_SLTIU,
                              II_SUB, II_SUBU, II_XOR, II_XORI, II_MFHI_MFLO,
                              II_MTHI_MTLO]>;

// ALU1: the shifter, conditional moves, branches, jumps and traps.
def : ItinRW<[R10KWriteALU1], [II_SLL, II_SLLV, II_SRA, II_SRAV, II_SRL,
                               II_SRLV, II_DSLL, II_DSLL32, II_DSLLV, II_DSRA,
                               II_DSRA32, II_DSRAV, II_DSRL, II_DSRL32,
                               II_DSRLV, II_MOVN, II_MOVZ, II_MOVF, II_MOVT,
                               II_B, II_BCC, II_BCCZ, II_BCCZAL, II_BCCZALS,
                               II_BC1F, II_BC1FL, II_BC1T, II_BC1TL, II_J,
                               II_JAL, II_JALR, II_JALR_HB, II_JALS, II_JALRS,
                               II_JR, II_JR_HB, II_TEQ, II_TEQI, II_TGE,
                               II_TGEI, II_TGEIU, II_TGEU, II_TLT, II_TLTI,
                               II_TLTU, II_TTLTIU, II_TNE, II_TNEI, II_TRAP,
                               II_BREAK, II_SYSCALL, II_SDBBP, II_ERET,
                               II_DERET, II_WAIT, II_EI, II_DI]>;

def : ItinRW<[R10KWriteNop], [II_SSNOP, II_EHB, II_PAUSE]>;

// Multiply and divide: LO's latency; the unit is held for the repeat rate.
class R10KWriteMulDiv<int lat, int busy>
    : SchedWriteRes<[R10KALU2, R10KMulDiv]> {
  let Latency = lat;
  let ReleaseAtCycles = [1, busy];
}
def R10KWriteMULT : R10KWriteMulDiv<5, 6>;
def R10KWriteMULTU : R10KWriteMulDiv<6, 7>;
def R10KWriteDMULT : R10KWriteMulDiv<9, 8>;
def R10KWriteDMULTU : R10KWriteMulDiv<10, 9>;
def R10KWriteDIV : R10KWriteMulDiv<34, 34>;
def R10KWriteDDIV : R10KWriteMulDiv<66, 66>;

def : ItinRW<[R10KWriteMULT], [II_MULT, II_MUL, II_MADD, II_MSUB]>;
def : ItinRW<[R10KWriteMULTU], [II_MULTU, II_MADDU, II_MSUBU]>;
def : ItinRW<[R10KWriteDMULT], [II_DMULT, II_DMUL]>;
def : ItinRW<[R10KWriteDMULTU], [II_DMULTU, II_DMULU]>;
def : ItinRW<[R10KWriteDIV], [II_DIV, II_DIVU]>;
def : ItinRW<[R10KWriteDDIV], [II_DDIV, II_DDIVU]>;

// Loads and stores
// ================

def R10KWriteLoad : SchedWriteRes<[R10KLS]> { let Latency = 2; }
def R10KWriteLoadFP : SchedWriteRes<[R10KLS]> { let Latency = 3; }
def R10KWriteStore : SchedWriteRes<[R10KLS]>;

def : ItinRW<[R10KWriteLoad], [II_LB, II_LBU, II_LH, II_LHU, II_LW, II_LWU,
                               II_LWL, II_LWR, II_LD, II_LDL, II_LDR, II_LL,
                               II_LLD]>;
def : ItinRW<[R10KWriteLoadFP], [II_LWC1, II_LDC1, II_LWXC1, II_LDXC1,
                                 II_LWC2, II_LDC2, II_LWC3, II_LDC3]>;
def : ItinRW<[R10KWriteStore], [II_SB, II_SH, II_SW, II_SWL, II_SWR, II_SD,
                                II_SDL, II_SDR, II_SC, II_SCD, II_SWC1,
                                II_SDC1, II_SWXC1, II_SDXC1, II_SWC2, II_SDC2,
                                II_SWC3, II_SDC3, II_PREF, II_SYNC, II_CACHE]>;

// Floating point
// ==============

def R10KWriteFPAdd : SchedWriteRes<[R10KFPAdd]> { let Latency = 2; }
def R10KWriteFPCvtFromInt : SchedWriteRes<[R10KFPAdd]> {
  let Latency = 4;
  let ReleaseAtCycles = [2];
}
def R10KWriteFPMul : SchedWriteRes<[R10KFPMul]> { let Latency = 2; }
def R10KWriteFPMAdd : SchedWriteRes<[R10KFPMul, R10KFPAdd]> {
  let Latency = 4;
}

def : ItinRW<[R10KWriteFPAdd], [II_ADD_S, II_ADD_D, II_SUB_S, II_SUB_D,
                                II_ABS, II_NEG, II_MOV_S, II_MOV_D,
                                II_C_CC_S, II_C_CC_D, II_CEIL, II_FLOOR,
                                II_ROUND, II_TRUNC, II_MOVF_S, II_MOVF_D,
                                II_MOVT_S, II_MOVT_D, II_MOVN_S, II_MOVN_D,
                                II_MOVZ_S, II_MOVZ_D]>;
def : ItinRW<[R10KWriteFPAdd], [II_CVT]>;
// Integer to single precision is the slow conversion.
def : InstRW<[R10KWriteFPCvtFromInt], (instrs CVT_S_W, CVT_S_L,
                                              PseudoCVT_S_L)>;
def : ItinRW<[R10KWriteFPMul], [II_MUL_S, II_MUL_D]>;
// The addend joins in the adder, two cycles in, so an accumulation chain of
// multiply-adds runs at the adder's latency: the addend of a multiply-add
// fed by another is needed two cycles late.
def R10KReadMAddAddend : SchedReadAdvance<2, [R10KWriteFPMAdd]>;
def : ItinRW<[R10KWriteFPMAdd, R10KReadMAddAddend, ReadDefault, ReadDefault],
             [II_MADD_S, II_MADD_D, II_MSUB_S, II_MSUB_D, II_NMADD_S,
              II_NMADD_D, II_NMSUB_S, II_NMSUB_D]>;

class R10KWriteFPDivSqrt<int lat, int busy>
    : SchedWriteRes<[R10KFPMul, R10KFPDivSqrt]> {
  let Latency = lat;
  let ReleaseAtCycles = [1, busy];
}
def R10KWriteDIV_S : R10KWriteFPDivSqrt<12, 14>;
def R10KWriteDIV_D : R10KWriteFPDivSqrt<19, 21>;
def R10KWriteSQRT_S : R10KWriteFPDivSqrt<18, 20>;
def R10KWriteSQRT_D : R10KWriteFPDivSqrt<33, 35>;

def : ItinRW<[R10KWriteDIV_S], [II_DIV_S, II_RECIP_S]>;
def : ItinRW<[R10KWriteDIV_D], [II_DIV_D, II_RECIP_D]>;
def : ItinRW<[R10KWriteSQRT_S], [II_SQRT_S, II_RSQRT_S]>;
def : ItinRW<[R10KWriteSQRT_D], [II_SQRT_D, II_RSQRT_D]>;

// Moves between the register files, and the FP control registers.
def R10KWriteMFC1 : SchedWriteRes<[R10KFPAdd]> { let Latency = 2; }
def R10KWriteMTC1 : SchedWriteRes<[R10KFPAdd]> { let Latency = 3; }
def : ItinRW<[R10KWriteMFC1], [II_MFC1, II_DMFC1, II_CFC1]>;
def : ItinRW<[R10KWriteMTC1], [II_MTC1, II_DMTC1, II_CTC1]>;

// System control
// ==============

def R10KWriteCOP0 : SchedWriteRes<[R10KALU1]> { let Latency = 2; }
def : ItinRW<[R10KWriteCOP0], [II_MFC0, II_MTC0, II_DMFC0, II_DMTC0,
                               II_MFC2, II_MTC2, II_DMFC2, II_DMTC2, II_CFC2,
                               II_CTC2, II_TLBP, II_TLBR, II_TLBWI,
                               II_TLBWR]>;

// Pseudo-instructions without an itinerary class
// ===============================================

def : InstRW<[R10KWriteALU], (instrs COPY)>;
def : InstRW<[R10KWriteNop], (instrs NOP)>;
def : InstRW<[R10KWriteALU1], (instrs ERet, PseudoIndirectBranch,
                                      PseudoIndirectBranch64, PseudoReturn,
                                      PseudoReturn64, RetRA, RDHWR, RDHWR64)>;

// The atomic read-modify-write loops expanded after register allocation: an
// ll, the operation and an sc, at least.
def R10KWriteAtomic : SchedWriteRes<[R10KLS, R10KALU]> { let Latency = 4; }
def : InstRW<[R10KWriteAtomic],
             (instregex "^ATOMIC_(SWAP|CMP_SWAP)_I(8|16|32|64)_POSTRA$")>;
def : InstRW<[R10KWriteAtomic],
             (instregex "^ATOMIC_LOAD_(ADD|SUB|AND|OR|XOR|NAND|MIN|MAX|UMIN|"
                        "UMAX)_I(8|16|32|64)_POSTRA$")>;

} // SchedModel = MipsR10000Model

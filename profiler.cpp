
#include <iostream>
#include <bitset>
#include <cstdint>
#include <cmath>
#include "profiler.h"
#include "drcctlib_vscodeex_format.h"

using namespace std;
using namespace DrCCTProf;

/*
    Tips: different integer types have distinct boundaries
    INT64_MAX, INT32_MAX, INT16_MAX, INT8_MAX
    INT64_MIN, INT32_MIN, INT16_MIN, INT8_MIN
*/

namespace runtime_profiling {

static inline int64_t
GetOpndIntValue(Operand opnd)
{
    if (opnd.type == OperandType::kImmediateFloat ||
        opnd.type == OperandType::kImmediateDouble ||
        opnd.type == OperandType::kUnsupportOpnd || opnd.type == OperandType::kNullOpnd) {
        return 0;
    }
    int64_t value = 0;
    switch (opnd.size) {
    case 1: value = static_cast<int64_t>(opnd.value.valueInt8); break;
    case 2: value = static_cast<int64_t>(opnd.value.valueInt16); break;
    case 4: value = static_cast<int64_t>(opnd.value.valueInt32); break;
    case 8: value = static_cast<int64_t>(opnd.value.valueInt64); break;
    default: break;
    }
    return value;
}

// implement your algorithm in this function
static inline bool
IntegerOverflow(Instruction *instr, uint64_t flagsValue)
{
       // add: Destination = Source0 + Source1
    if (instr->getOperatorType() == OperatorType::kOPadd) {
        Operand srcOpnd0 = instr->getSrcOperand(0);
        Operand srcOpnd1 = instr->getSrcOperand(1);
        Operand dstOpnd = instr->getDstOperand(0);
        std::bitset<64> bitFlagsValue(flagsValue);

        int64_t source0 = GetOpndIntValue(srcOpnd0);
        int64_t source1 = GetOpndIntValue(srcOpnd1);

        if (srcOpnd0.size == 1 && srcOpnd1.size == 1 && dstOpnd.size == 1) {
            if (((source0 > 0) && (source1 > (INT8_MAX - source0))) || ((source0 < 0) && (source1 < (INT8_MIN - source0)))) {
                return true;
            }
        }
        if (srcOpnd0.size == 2 && srcOpnd1.size == 2 && dstOpnd.size == 2) {
            if (((source0 > 0) && (source1 > (INT16_MAX - source0))) || ((source0 < 0) && (source1 < (INT16_MIN - source0)))) {
                return true;
            }
        }
        if (srcOpnd0.size == 4 && srcOpnd1.size == 4 && dstOpnd.size == 4) {
            if (((source0 > 0) && (source1 > (INT32_MAX - source0))) || ((source0 < 0) && (source1 < (INT32_MIN - source0)))) {
                return true;
            }
        }
        if (srcOpnd0.size == 8 && srcOpnd1.size == 8 && dstOpnd.size == 8) {
            if (((source0 > 0) && (source1 > (INT64_MAX - source0))) || ((source0 < 0) && (source1 < (INT16_MIN - source0)))) {
                return true;
            }
        }
    }
    // sub: Destination = Source0 - Source1
    if (instr->getOperatorType() == OperatorType::kOPsub) {
        Operand srcOpnd0 = instr->getSrcOperand(0);
        Operand srcOpnd1 = instr->getSrcOperand(1);
        Operand dstOpnd = instr->getDstOperand(0);
        std::bitset<64> bitFlagsValue(flagsValue);
        
        int64_t source0 = GetOpndIntValue(srcOpnd0);
        int64_t source1 = GetOpndIntValue(srcOpnd1);
        
        if (srcOpnd0.size == 1 && srcOpnd1.size == 1 && dstOpnd.size == 1) {
            if (((source0 > 0) && (source1 < (INT8_MIN + source0))) || ((source0 < 0) && (source1 > (INT8_MAX + source0)))) {
                return true;
            }
        }
        if (srcOpnd0.size == 2 && srcOpnd1.size == 2 && dstOpnd.size == 2) {
            if (((source0 > 0) && (source1 < (INT16_MIN + source0))) || ((source0 < 0) && (source1 > (INT16_MAX + source0)))) {
                return true;
            }
        }
        if (srcOpnd0.size == 4 && srcOpnd1.size == 4 && dstOpnd.size == 4) {
            if (((source0 > 0) && (source1 < (INT32_MIN + source0))) || ((source0 < 0) && (source1 > (INT32_MAX + source0)))) {
                return true;
            }
        }
        if (srcOpnd0.size == 8 && srcOpnd1.size == 8 && dstOpnd.size == 8) {
            if (((source0 > 0) && (source1 < (INT64_MIN + source0))) || ((source0 < 0) && (source1 > (INT64_MAX + source0)))) {
                return true;
            }
        }
    }
    // shl: Destination = Source0 << Source1
    if (instr->getOperatorType() == OperatorType::kOPshl) {
        Operand srcOpnd0 = instr->getSrcOperand(0);
        Operand srcOpnd1 = instr->getSrcOperand(1);
        Operand dstOpnd = instr->getDstOperand(0);
        std::bitset<64> bitFlagsValue(flagsValue);

        int64_t source0 = GetOpndIntValue(srcOpnd0);
        int64_t source1 = GetOpndIntValue(srcOpnd1);
	
	__int128_t dest = source0;
	
	for (int i=0 ;i<source1; i++) {
	    dest = dest*2;
	}

        if (srcOpnd0.size == 1) {
            if (dest > INT8_MAX || dest < INT8_MIN) {
                    return true;
            }
        }
        if (srcOpnd0.size == 2) {
            if (dest > INT16_MAX || dest <INT16_MIN) {
                    return true;
            }
        }
	if (srcOpnd0.size == 4) {
            if (dest > INT32_MAX || dest <INT32_MIN) {
                    return true;
            }
        }
	if (srcOpnd0.size == 8) {
            if (dest > INT64_MAX || dest <INT64_MIN) {
                    return true;
            }
        }
    }
    return false;
}

void
OnAfterInsExec(Instruction *instr, context_handle_t contxt, uint64_t flagsValue,
                        CtxtContainer *ctxtContainer)
{
    if (IntegerOverflow(instr, flagsValue)) {
        ctxtContainer->addCtxt(contxt);
    }
    // add: Destination = Source0 + Source1
    if (instr->getOperatorType() == OperatorType::kOPadd) {
        Operand srcOpnd0 = instr->getSrcOperand(0);
        Operand srcOpnd1 = instr->getSrcOperand(1);
        Operand dstOpnd = instr->getDstOperand(0);
        std::bitset<64> bitFlagsValue(flagsValue);

        cout << "ip(" << hex << instr->ip << "):"
             << "add " << dec << GetOpndIntValue(srcOpnd0) << " "
             << GetOpndIntValue(srcOpnd1) << " -> " << GetOpndIntValue(dstOpnd) << " "
             << bitFlagsValue << endl;
    }
    // sub: Destination = Source0 - Source1
    if (instr->getOperatorType() == OperatorType::kOPsub) {
        Operand srcOpnd0 = instr->getSrcOperand(0);
        Operand srcOpnd1 = instr->getSrcOperand(1);
        Operand dstOpnd = instr->getDstOperand(0);
        
        std::bitset<64> bitFlagsValue(flagsValue);

        cout << "ip(" << hex << instr->ip << "):"
             << "sub " << dec << GetOpndIntValue(srcOpnd0) << " "
             << GetOpndIntValue(srcOpnd1) << " -> " << GetOpndIntValue(dstOpnd) << " "
             << bitFlagsValue << endl;
    }
    // shl: Destination = Source0 << Source1
    if (instr->getOperatorType() == OperatorType::kOPshl) {
        Operand srcOpnd0 = instr->getSrcOperand(0);
        Operand srcOpnd1 = instr->getSrcOperand(1);
        Operand dstOpnd = instr->getDstOperand(0);
        std::bitset<64> bitFlagsValue(flagsValue);

        cout << "ip(" << hex << instr->ip << "):"
             << "shl " << dec << GetOpndIntValue(srcOpnd0) << " "
             << GetOpndIntValue(srcOpnd1) << " -> " << GetOpndIntValue(dstOpnd) << " "
             << bitFlagsValue << endl;
    }
}

void
OnBeforeAppExit(CtxtContainer *ctxtContainer)
{
    Profile::profile_t *profile = new Profile::profile_t();
    profile->add_metric_type(1, "", "integer overflow occurrence");

    std::vector<context_handle_t> list = ctxtContainer->getCtxtList();
    for (size_t i = 0; i < list.size(); i++) {
        inner_context_t *cur_ctxt = drcctlib_get_full_cct(list[i]);
        profile->add_sample(cur_ctxt)->append_metirc((uint64_t)1);
        drcctlib_free_full_cct(cur_ctxt);
    }
    profile->serialize_to_file("integer-overflow-profile.drcctprof");
    delete profile;

    file_t profileTxt = dr_open_file("integer-overflow-profile.txt", DR_FILE_WRITE_OVERWRITE | DR_FILE_ALLOW_LARGE);
    DR_ASSERT(profileTxt != INVALID_FILE);
    for (size_t i = 0; i < list.size(); i++) {
        dr_fprintf(profileTxt, "INTEGER OVERFLOW\n");
        drcctlib_print_backtrace_first_item(profileTxt, list[i], true, false);
        dr_fprintf(profileTxt, "=>BACKTRACE\n");
        drcctlib_print_backtrace(profileTxt, list[i], false, true, -1);
        dr_fprintf(profileTxt, "\n\n");
    }
    dr_close_file(profileTxt);
}

}
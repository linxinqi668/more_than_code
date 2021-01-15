/*
 * @Author: your name
 * @Date: 2020-10-28 12:50:54
 * @LastEditTime: 2020-11-06 15:50:48
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /NEMU2020/nemu/src/cpu/exec/sub/sub-template.h
 */
#include "cpu/exec/template-start.h"

#define instr sub

static void do_execute() {
    // dest - src

    DATA_TYPE_S src;
    if (op_src->type == OP_TYPE_IMM && op_src->size == 1) {
        // extended_src
        int8_t src_ = op_src->val;
        src = src_;
    } else
        src = op_src->val;

    DATA_TYPE_S minus_res = op_dest->val - src;

    // debug..
    // if (cpu.eip == 0x100354) {
    //     printf("op_size is : %d\n", (int)op_src->size);
    //     printf("dest is : %x\n", op_dest->val);
    //     printf("dest reg is: %s\n", regsl[op_dest->reg]);
    //     printf("src is : %x\n", src);
    // }
    // write
    OPERAND_W(op_dest, minus_res);

    // set ZF
    cpu.EFLAGS.ZF = (minus_res == 0) ? 1 : 0;

    // set PF
    uint8_t low_byte = minus_res;
    uint32_t count;
    for (count = 0; low_byte; ++count)
        low_byte &= (low_byte - 1); // 不断清除右边的1
    cpu.EFLAGS.PF = (count % 2 == 0) ? 1 : 0;


    // set SF
    cpu.EFLAGS.SF = (DATA_TYPE)minus_res >> (DATA_BYTE * 8 - 1);

    // set OF in subtraction.
    if (
        !(((DATA_TYPE)minus_res >> (DATA_BYTE * 8 - 1)) ^
        ((DATA_TYPE)src >> (DATA_BYTE * 8 - 1))) &&
        (((DATA_TYPE)op_dest->val >> (DATA_BYTE * 8 - 1)) ^
        ((DATA_TYPE)src >> (DATA_BYTE * 8 - 1)))
    )
        cpu.EFLAGS.OF = 1;
    else
        cpu.EFLAGS.OF = 0;

    // set CF in subtraction.
    if ((DATA_TYPE)op_dest->val < (DATA_TYPE)src)
        cpu.EFLAGS.CF = 1;
    else
        cpu.EFLAGS.CF = 0;

    print_asm_template2();
}

make_instr_helper(ib2rm);
make_instr_helper(rm2r);
make_instr_helper(r2rm);
make_instr_helper(i2rm);


make_helper(concat(sub_i2A_, SUFFIX)) {
    // decode. eip -> imm
    int len = concat(decode_i_, SUFFIX)(eip + 1);

    DATA_TYPE_S src = op_src->val;
    DATA_TYPE_S dest = REG(R_EAX);
    DATA_TYPE_S minus_res = dest - src;

    if (eip == 0x100823)
    {
        printf("dest: %d\n", dest);
        printf("src: %d\n", src);
        printf("res: %d\n", minus_res);
        getchar();
    }

    REG(R_EAX) = minus_res;

    // set ZF
    cpu.EFLAGS.ZF = (minus_res == 0) ? 1 : 0;

    // set PF
    uint8_t low_byte = minus_res;
    uint32_t count;
    for (count = 0; low_byte; ++count)
        low_byte &= (low_byte - 1); // 不断清除右边的1
    cpu.EFLAGS.PF = (count % 2 == 0) ? 1 : 0;


    // set SF
    cpu.EFLAGS.SF = (DATA_TYPE)minus_res >> (DATA_BYTE * 8 - 1);

    // set OF in subtraction.
    if (
        !(((DATA_TYPE)minus_res >> (DATA_BYTE * 8 - 1)) ^
        ((DATA_TYPE)src >> (DATA_BYTE * 8 - 1))) &&
        (((DATA_TYPE)dest >> (DATA_BYTE * 8 - 1)) ^
        ((DATA_TYPE)src >> (DATA_BYTE * 8 - 1)))
    )
        cpu.EFLAGS.OF = 1;
    else
        cpu.EFLAGS.OF = 0;

    // set CF in subtraction.
    if ((DATA_TYPE)dest < (DATA_TYPE)src)
        cpu.EFLAGS.CF = 1;
    else
        cpu.EFLAGS.CF = 0;
    print_asm_template2();

    return 1 + len;
}



#include "cpu/exec/template-end.h"

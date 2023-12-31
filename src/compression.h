#ifndef COMPRESSION_H
#define COMPRESSION_H

/* Your code here... */
#define OPCODE 0x7f

typedef enum{
    R  = 0x33,
    I  = 0x13,
    LI = 0x03,
    S  = 0x23,
    SB = 0x63,
    JI = 0x67,
    U  = 0x37,
    UJ = 0x6f,
    UNVAILD = 0x00
}RISCV_Format_e;

typedef enum{
    CR ,
    CI ,
    CL ,
    CS_T1 ,
    CS_T2 ,
    CB_T1 ,
    CB_T2 ,
    CJ,
    NONE
}RVC_Format_e;

typedef enum{
    B_J_INCOMPRESSIBLE,
    COMPRESSIBLE,
    INCOMPRESSIBLE
}cmd_state_t;

typedef struct{
    uint32_t cmd;
    cmd_state_t  state;
    RISCV_Format_e format;
    RVC_Format_e  c_format;
}cmd_info_t;

#define REGISTER 0x1f
#define FUNCT3 0x07
#define FUNCT7 0x7f
#define IMM20 0xfffff
#define IMM12 0xfff
#define IMM7  0x7f
#define IMM5  0x1f
#define SIGN_ALL 0xffffffff

cmd_info_t * compress(uint32_t *);
#endif
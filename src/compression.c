#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "compression.h"
#include "utils.h"

void R_compress(cmd_info_t * cmd_info){
    (void) cmd_info;
}

/*check add,and,or,xor,sub*/
cmd_state_t R_check(uint32_t cmd){
    /*get parameters according to the format*/
    uint32_t rd = (cmd>>7)&REGISTER;
    uint32_t funct3 = (cmd>>12)&FUNCT3;
    uint32_t rs1 = (cmd>>15)&REGISTER;
    uint32_t rs2 = (cmd>>20)&REGISTER;
    uint32_t funct7 = (cmd>>25)&FUNCT7;
    /*according to funct3 to check what cmd it actually is*/
    switch(funct3){
        /*add or sub*/
        case 0x0:
            /*add*/
            if(funct7 == 0x0){
                /*c.add*/
                if(rd == rs1 && rd && rs2){
                    return COMPRESSIBLE;
                }
                /*c.mv*/
                if(rs1 == 0 && rd && rs2){
                    return COMPRESSIBLE;
                }
                /*cannot be compressed*/
                return INCOMPRESSIBLE;
            }
            /*sub*/
            else{
                if(rd==rs1)
                    return COMPRESSIBLE;
                return INCOMPRESSIBLE;
            }
            /*otherwise, it is incompressible*/
            break;
        /*xor,and,or share the same condition when compressible*/
        case 0x4:
        case 0x5:
        case 0x6:
            /*compressible only when rd is same with rs1*/
            if(rd==rs1)
                return COMPRESSIBLE;
            return INCOMPRESSIBLE;
            break;
        default:
            return INCOMPRESSIBLE;
    }
}

cmd_state_t I_check(uint32_t cmd){
    (void) cmd;
    return 0;
}

/*check lw*/
cmd_state_t LI_check(uint32_t cmd){
    /*get parameters according to format*/
    uint32_t rd = (cmd>>7)&REGISTER;
    uint32_t rs1 = (cmd>>15)&REGISTER;
    int32_t  imm12 = (cmd>>20)&IMM12;
    uint32_t funct3 = (cmd>>12)&FUNCT3;
    /*check whether it is lw*/
    if(funct3 != 0x2)
        return INCOMPRESSIBLE;
    /*check whether rs1 is between x8-x15*/
    if(rs1>=16 || rs1<=7)
        return INCOMPRESSIBLE;
    /*check whether rd is between x8-x15*/
    if(rd>=16 || rd<=7)
        return INCOMPRESSIBLE;
    /*incompressible if offset is negative or the least two significant bits is non-zero */
    if(imm12 < 0 || (imm12>>10))
        return INCOMPRESSIBLE;
    /*otherwise compressible*/
    return COMPRESSIBLE;
}

/*check sw*/
cmd_state_t S_check(uint32_t cmd){
    /*get parameters according to format*/
    int32_t imm12 = 0;
    uint32_t funct3 = (cmd>>12)&FUNCT3;
    uint32_t rs1 = (cmd>>15)&REGISTER;
    uint32_t rs2 = (cmd>>20)&REGISTER;
    /*get imm from two part of the cmd*/
    imm12 |= (cmd>>7)&IMM5;
    imm12 |= (cmd>>25)&IMM7;
    /*check whether the cmd is sw*/
    if(funct3 != 0x2)
        return INCOMPRESSIBLE;
    /*check whether rs1 is between x8-x15*/
    if(rs1>=16 || rs1<=7)
        return INCOMPRESSIBLE;
    /*check whether rs2 is between x8-x15*/
    if(rs2>=16 || rs2<=7)
        return INCOMPRESSIBLE;
    /*incompressible if offset is negative or the least two significant bits is non-zero */
    if(imm12 < 0 || (imm12>>10))
        return INCOMPRESSIBLE;
    /*otherwise compressible*/
    return COMPRESSIBLE;
}

cmd_state_t SB_check(uint32_t cmd){
    (void) cmd;
    return 0;
}

/*check jalr*/
cmd_state_t JI_check(uint32_t cmd){
    /*get parameters according to the format*/
    uint32_t funct3 = (cmd>>12)&FUNCT3;
    uint32_t rd = (cmd>>7)&REGISTER;
    uint32_t rs1 = (cmd>>15)&REGISTER;
    int32_t  imm12 = (cmd>>20)&IMM12;
    /*check funct3 to determine whether the command is jalr*/
    if(funct3 != 0x0)
        return INCOMPRESSIBLE;
    /*incompressible if imm is non-zero or rs1 is x0 or rd is not x0 or x1 */
    if(imm12 || !rs1 || rd>=2){
        return INCOMPRESSIBLE;
    }
    return COMPRESSIBLE;
}

cmd_state_t U_check(uint32_t cmd){
    /*
    uint32_t rd = (cmd>>7)&REGISTER;
    int32_t  imm20 = (cmd>>12)&IMM20;
    if (!imm20 && !rd && rd!=2)
        return INCOMPRESSIBLE;
    if(imm20>0 && !(imm20>>5)){
        return COMPRESSIBLE;
    }
    else if (imm20 < 0){

    }
    else{
        return INCOMPRESSIBLE;
    }
    */
    (void) cmd;
    return 0;
}

cmd_state_t UJ_check(uint32_t cmd){
    (void) cmd;
    return 0;
}

void handle_compressible(cmd_info_t * cmd_info){
    (void) cmd_info;
}

void handle_unsure(cmd_info_t * cmd_info){
    (void) cmd_info;
}

void format_compressible_check(cmd_info_t * cmd_info,uint32_t * cmd_data){
    uint32_t i =0;
    /*transvse the whole cmd to check every one*/
    for(i=0;i<cmd_num;i++){
        cmd_info[i].cmd = cmd_data[i];
        switch(cmd_data[i] & OPCODE){
            /*R type, check whether compressible*/
            case R:
                cmd_info[i].format = R;
                cmd_info[i].state = R_check(cmd_info[i].cmd);
                break;
            /*I type, check whether compressible*/
            case I:
                cmd_info[i].format = I;
                cmd_info[i].state = I_check(cmd_info[i].cmd);
                break;
            /*LI type, check whether compressible*/
            case LI:
                cmd_info[i].format = LI;
                cmd_info[i].state = LI_check(cmd_info[i].cmd);
                break;
            /*U type, check whether compressible*/
            case U:
                cmd_info[i].format = U;
                cmd_info[i].state = U_check(cmd_info[i].cmd);
                break;
            /*S type, check whether compressible*/
            case S:
                cmd_info[i].format = S;
                cmd_info[i].state = S_check(cmd_info[i].cmd);
                break;
            /*SB type, check whether compressible*/
            case SB:
                cmd_info[i].format = SB;
                cmd_info[i].state = SB_check(cmd_info[i].cmd);
                break;
            /*UJ type, check whether compressible*/
            case UJ:
                cmd_info[i].format = UJ;
                cmd_info[i].state = UJ_check(cmd_info[i].cmd);
                break;
            /*JI type, check whether compressible*/
            case JI:
                cmd_info[i].format = JI;
                cmd_info[i].state = JI_check(cmd_info[i].cmd);
                break;
            /*otherwise*/
            default:
                cmd_info[i].format = UNVAILD;
                cmd_info[i].state = INCOMPRESSIBLE;
        }
    }
    return ;
}

cmd_info_t * compress(uint32_t * cmd_data){
    /*get a cmd_info array to store all the information of the cmd*/
    cmd_info_t * cmd_info;
    cmd_info = (cmd_info_t *)calloc(sizeof(cmd_info_t),cmd_num);
    /*check their type and whether compressible*/
    format_compressible_check(cmd_info,cmd_data);
    /*unuse the cmd_data*/
    free(cmd_data);
    return NULL;
}

/* Your code here... */


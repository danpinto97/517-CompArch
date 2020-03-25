#include <stdio.h>
#include "shell.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>


uint8_t op;
uint32_t instruction;
uint32_t addr_rs,addr_rt,addr_rd;
uint32_t rs = 0;
uint32_t rt = 0;
uint32_t rd = 0; //to save the register value
//for use in signed arithmetic
int32_t signed_rs = 0;
int32_t signed_rt = 0;
int32_t signed_rd = 0;
uint8_t func;
uint16_t itemp;         //for save immidiate number

  //please take a loot at https://inst.eecs.berkeley.edu/~cs61c/resources/MIPS_Green_Sheet.pdf

void fetch()
{
    printf("fetch ");
    instruction = mem_read_32(CURRENT_STATE.PC); //the instruction
    NEXT_STATE.PC=CURRENT_STATE.PC+4;

}


/*
OPCODE List
j = 2
addi = 8
addiu = 9

*/

void decode()
{
//printf("decode ");
    op=instruction>>26; //find the 31-26 bit


  //  printf("the op: %x ",op);
    // if op==0, it is R type
    if(op==0){//add instruction

        addr_rs=(instruction>>21) & 0x1f;
        addr_rt=(instruction>>16) & 0x1f;
        addr_rd=(instruction>>11) & 0x1f;
        rs= (uint32_t)CURRENT_STATE.REGS[addr_rs];   //2^5=32
        rt= (uint32_t)CURRENT_STATE.REGS[addr_rt];
        signed_rs=CURRENT_STATE.REGS[addr_rs];   //2^5=32
        signed_rt=CURRENT_STATE.REGS[addr_rt];
      //  rd=CURRENT_STATE.REGS[addr_rd];
        func= instruction & 0x3f;
    }



    if(op==8){          //addi:001000
        printf("addi instruction \n");
        addr_rs=(instruction>>21) & 0x1f;
        addr_rt=(instruction>>16) & 0x1f;
        rs= (uint32_t)CURRENT_STATE.REGS[addr_rs];   //2^5=32
        rt= (uint32_t)CURRENT_STATE.REGS[addr_rt];
        signed_rs=CURRENT_STATE.REGS[addr_rs];   //2^5=32
        signed_rt=CURRENT_STATE.REGS[addr_rt];
        itemp=instruction & 0x0000ffff;

        signed_rt = signed_rs + itemp;

        if( (signed_rs > 0 && itemp > 0 && signed_rt < 0) ||  (signed_rs < 0 && itemp < 0 && signed_rt > 0)){
            printf("overflow trap \n");
            NEXT_STATE.FLAG_V = 1;
        }
        NEXT_STATE.REGS[addr_rt]=signed_rt; // It is fine if you just execute here instead of executing in execute() function.
    }
    if (op == 9){
      printf("addiu instruction \n");
      addr_rs=(instruction>>21) & 0x1f;
      addr_rt=(instruction>>16) & 0x1f;
      rs= (uint32_t)CURRENT_STATE.REGS[addr_rs];   //2^5=32
      rt= (uint32_t)CURRENT_STATE.REGS[addr_rt];
      signed_rs=CURRENT_STATE.REGS[addr_rs];   //2^5=32
      signed_rt=CURRENT_STATE.REGS[addr_rt];
      itemp=instruction & 0x0000ffff;

      signed_rt = signed_rs + itemp;
      if((signed_rt < itemp) || (signed_rt < signed_rs)){
          NEXT_STATE.FLAG_C = 1;
      }
      NEXT_STATE.REGS[addr_rt] = signed_rt;


    }
}

void execute()
{
printf("execute");
if(op==0){
    switch (func) {
        case 32:      //add:100000
            printf("add instruction \n");
            rd=0;
            signed_rd = signed_rs + signed_rt;
            //check that result of two same-signed numbers has same sign
            if( (signed_rt > 0 && signed_rs > 0 && signed_rd < 0) ||  (signed_rt < 0 && signed_rs < 0 && signed_rd > 0)){
                printf("overflow trap \n");
                NEXT_STATE.FLAG_V = 1;
            }

            NEXT_STATE.REGS[addr_rd] = signed_rd;
            break;

        case 33:
          printf("addu instruction \n");

          signed_rd = signed_rs + signed_rt;
          if((signed_rd < signed_rt) || (signed_rd < signed_rs)){
              NEXT_STATE.FLAG_C = 1;
          }
          NEXT_STATE.REGS[addr_rd] = signed_rd;
          break;

        case 34:
          printf("sub instruction \n");

          signed_rd = signed_rs - signed_rt;
          if( (signed_rt > 0 && signed_rs > 0 && signed_rd < 0) ||  (signed_rt < 0 && signed_rs < 0 && signed_rd > 0)){
              printf("overflow trap \n");
              NEXT_STATE.FLAG_V = 1;
          }
          NEXT_STATE.REGS[addr_rd] = signed_rd;
          break;

        case 35:
          printf("Subu instruction \n");
          signed_rd = signed_rs - signed_rt;
          NEXT_STATE.REGS[addr_rd] = signed_rd;
          break;

        case 36:
          printf("And instruction \n");
          signed_rd = signed_rs & signed_rt;
          NEXT_STATE.REGS[addr_rd] = signed_rd;
          break;

        case 12:   //system call:001100
            if(CURRENT_STATE.REGS[2]==10){  //v0==10 then exit
                printf("systemcall: exit\n");
                RUN_BIT=FALSE;
            }
            if(CURRENT_STATE.REGS[2]==1){   //v0==1: print int
                rt=CURRENT_STATE.REGS[4];   //a0 is 4th register
                printf("\n print the number:   %d \n ",rt);

            }
            break;
        default:
            break;
    }

}
}
void process_instruction()
{
    /* execute one instruction here. You should use CURRENT_STATE and modify
     * values in NEXT_STATE. You can call mem_read_32() and mem_write_32() to
     * access memory. */
    fetch();
    decode();
    execute();

}

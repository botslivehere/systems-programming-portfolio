/*************************************************************/
/*Software Engineering for SRV and IV (Part 1) (2022-2023)   */
/*Task 1.0 - Lab-4. Interrupt Handling in real (ASM FILE)    */
/*Created by Avdzhyan A.E. KTbo2-9  Date:10.Apr.23           */
/*************************************************************/
#include <def21060.h>
#include <asm_sprt.h>
#include "const.h"

.global _SetupPORTS;
.global _ReadIOPReg;
.global _IRQ_Service;

.extern _Duplication;


/*************************************************/
.section/dm seg_dmda;

.var OutBuffer1[N];
.var OutBuffer2[N];
.var TCB_output1[8]=0,0,0,OutBuffer2-0x20000,TCB_output2+7,@OutBuffer1,1,OutBuffer1;
.var TCB_output2[8]=0,0,0,OutBuffer1-0x20000,TCB_output1+7,@OutBuffer2,1,OutBuffer2;
/*************************************************/


/*************************************************/
.section/pm seg_pmco;
//void SetupPORTS(int, int,int);
// FUNCTION setup current ports
//  R4 - 0 - in (LINK) / 1 - out (SPORT0)
//  R8 -  STCTL / LAR
//  R12 - DIVISOR / LCTL
_SetupPORTS:
    R4=PASS R4;
    IF NE JUMP TRANSMIT;
		//recieve
	//setting Link-port
	//LAR:  LBUF <-> LPORT	
	dm(LAR)=r8;
	//LCOM
	r4=0x00000000;		//LCOM: 0
	dm(LCOM)=R4;		// odinarnaya chastota
	//LBUF
	dm(LCTL)=r12;
	jump finish;

TRANSMIT:
    DM(TDIV0)=R12; //divisor
    
    R12=TCB_output1+7;//tcb_block
    DM(CP2)=R12;
    
    DM(STCTL0)=R8;

finish:
    I12=dm(-1,I6);
    JUMP (M14,I12)(DB);
		NOP;
		RFRAME;
		
_SetupPORTS.end:
nop;

/*************************************************/
//ReadIOPReg(int) 
//Function for returning current buffer TCB BLOCK
_ReadIOPReg:
    puts=I8; //to stack
    
	I8=R4;
    R0=pm(I8,M13);
    
    I8=gets(1); // from stack
    alter(1); // clear stack

    leaf_exit;

_ReadIOPReg.end:   nop;


/*************************************************/
//Function for working with Interrupts
//_IRQ_Service(int)
_IRQ_Service:
    save_reg;//save in stack r0..r15
    
//r0 = current output buffer
   r0=dm(GP2);
   r12=0x20000;
   r0=r0+r12;
    
    //GET current X
   	r4=dm(LBUF2);//get X
   	f4=float r4;
   	
   	//Compute
   	f8 = 1.0/2.0; // 0.5
   	f4 = f4	*f8; // f4 = X/2
    
    //void Duplication(float start_value,float *dst_buffer,int length){
	//r4 = start_value , r8 = dst_buffer , r12 = length
		r8=r0;
   		r12=N;
   		ccall(_Duplication);
   		
	restore_reg; //RESTORE R0-R15
	leaf_exit;
_IRQ_Service.end: nop;
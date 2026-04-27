/*************************************************************/
/*Software Engineering for SRV and IV (Part 1) (2022-2023)   */
/*Task 1.0 - Lab-2. Interrupt Handling                       */
/*Created by Avdzhyan A.E. KTbo2-9  Date:27.Mar.23           */
/*************************************************************/

#include "def21060.h"

#define N 4
#define c_astr 2
#define Left -20
#define Right 32

//------------------------------------------------------
//Ports
.SECTION/DM     dm_port; 
.VAR in;
.VAR out;
//------------------------------------------------------


//------------------------------------------------------
// PM Vector
.SECTION/PM     pm_data;
//------------------------------------------------------

//------------------------------------------------------
// DM Vector
.SECTION/dm     dm_data; 
//------------------------------------------------------

//------------------------------------------------------
.SECTION/PM     pm_irq_svc;
		nop; //256 bit prog.
		jump start; 
		nop;
		nop;
//------------------------------------------------------

//------------------------------------------------------
.SECTION/PM     pm_irq_tmz;
	call compute(db);
		R2 = DM(in);
		r7=c_astr;
	RTI;  
//------------------------------------------------------

//------------------------------------------------------
.SECTION/PM     pm_code;
start:

	r0=0; //delta Y
	r8=0; //Y
	r5=1;
	r6=-1;
	r10=Left;
	r11=Right;
	
//TIMER and activate pm_irq_tmz
TPERIOD = 1000;
BIT SET MODE1 IRPTEN;
BIT SET MODE2 TIMEN;
BIT SET IMASK TMZHI;

//  60MGHZ/ (label wait)(TPERIOD - TCOUN) = CHASTOA DESKITIZACII ;
 wait: idle;
jump wait;

//------------------------------------------------------
//PP for count delta modul. first
//r4 - code bit 1/0 (+1/-1),r1 = delta2(Y)
//R0 - delta Y, R8 - Y,R12 = output R8 with destination.
//Rewrite: R5,R6,R10,R11,R4,R1,R0,R8,R12,R2
compute:
	
	lcntr=N,do xxx until lce;
	
		call delta_sign(db);
			r4 = r2 and r5;
			r4 = pass r4;
		
		r1 = r7 * r4 (ssi);//delta2 Y
		r0 = r0 + r1;  //Delta Y
		r8 = r8 + r0; //Y
		
		call fun_out(db);
			r12 = r8; //Y for output =)
			r2=lshift r2 by r6;
			
		xxx:dm(out)=r12;//3 intsr. != call
rts;
//------------------------------------------------------

//------------------------------------------------------
//PP for count delta SIGN
//r4 - code bit 1/0 (+1/-1)
//Rewrite: R4.
delta_sign:
	rts(db);
		if eq r4 = r6;
		if ne r4 = r5;
//------------------------------------------------------		

//------------------------------------------------------
//PP for output
//r8 - Y , r10 =Left dest.,r11 = right dest.
//r12 = r8 for output with dest.
//Rewrite: r12.
fun_out:
		comp(r10,r8);
		if GT r12=r10;
	rts(db);
		comp(r11,r8);
		if LT r12=r11;
//------------------------------------------------------		
/*************************************************************/
/*Software Engineering for SRV and IV (Part 1) (2022-2023)   */
/*Task 1.0 - Lab-3. Interrupt Handling / upscaling           */
/*Created by Avdzhyan A.E. KTbo2-9  Date:03.Apr.23           */
/*************************************************************/

#include "def21060.h"
#define K 4
#define M 2

//------------------------------------------------------
// DM Vector
.SECTION/dm     dm_data;
.VAR in_buf1[M];
.VAR in_buf2[M];
.var in_select[2]= in_buf1,in_buf2;

.VAR TCB_Block1[8]=0,0,0,0,//first init
					TCB_Block2+7,//next buff
					@in_buf1,//length
					1,//modify
					in_buf1;//addres
					
.VAR TCB_Block2[8]=0,0,0,0,//first init
					TCB_Block1+7,//next buff
					@in_buf2,//length
					1,//modify
					in_buf2;//addres
					
.var out_buf1[K*M];
.var out_buf2[K*M];					
.var out_select[2] = out_buf2,out_buf1;

.VAR TCB_BlockOut1[8]=0,0,0,0,//first init
					TCB_BlockOut2+7,//next buff
					@out_buf1,//length
					1,//modify
					out_buf1;//addres
					
.VAR TCB_BlockOut2[8]=0,0,0,0,//first init
					TCB_BlockOut1+7,//next buff
					@out_buf2,//length
					1,//modify
					out_buf2;//addres
					
//------------------------------------------------------

//------------------------------------------------------
// PM Vector
.SECTION/PM     pm_data;  
.VAR X_astr[K*M];
//------------------------------------------------------

//------------------------------------------------------
.SECTION/PM     pm_irq_svc;
		nop; //256 bit prog.
		jump start; 
		nop;
		nop;
//------------------------------------------------------

//------------------------------------------------------
.SECTION/PM     pm_irq_spr0;
		m0=r10;
		jump compute(db);
			I0=DM(M0,I1);//Curent input buffer
			I7=DM(M0,I5);//Curent output buffer
//------------------------------------------------------

//------------------------------------------------------
.SECTION/PM     pm_code;
start:

	// 2 input buffers
	i1=in_select;
	
	// 2 output buffers
	i5=out_select;
	
	b9=X_astr;// buffer for X`(i)
	l9=@X_astr;
	M9=1;
// setting of DMA-channel #0 input SPORT0
R0= TCB_Block1+7;
dm(CP0)=R0;
//setting of SPOT0 Receive
R0=0x000C05F1;  //spen
			  //slen 31 (na samom dele = 32)
			  //ICLK =1;
			  //DEN, SCHEN =1;
dm(SRCTL0)=R0;


// setting of DMA-channel #2 output SPORT0
R0= TCB_BlockOut1+7;
dm(CP2)=R0;
//setting of SPOT0 Transmit
R0=0x000C05F1;  //spen
			  //slen 31 (na samom dele = 32)
			  //ICLK =1;
			  //DEN, SCHEN =1;
dm(STCTL0)=R0;



//setting Link-port
r0=0x0003FEBF;		//LAR:  LBUF2 <-> LPORT2	
dm(LAR)=r0;			//

r0=0x00000000;		//LCOM: 0

dm(LCOM)=r0;		// odinarnaya chastota
r0=0x00000100;		//LBUF2: L2EN=1,L2TRAN=0;
dm(LCTL)=r0;		//

R0=0; // Last X.
M1=1;
f15=1.0/K;
r10=0;
R13=K-1;

BIT SET IMASK SPR0I;
BIT SET MODE1 IRPTEN;

 wait: idle;
jump wait;

//------------------------------------------------------
//PP for upscaling
//f2 = Step of interpolation,f4 = X`(I)
//r0 = X(i) or X(i-1) ;r1 = X(i)
//r13 = ch-k if k==1;
//f5 = Z(i),r5= Z`(i)
//Rewrite: f2,f0,f4,r1,f5,r14,r10;
compute:
f3=dm(LBUF2);//Y(i)

lcntr=M,do xxx until lce;
	
	call calculate(db);
		r1=dm(i0,m1);//X(i)
		r2= r1 - r0;
		
	r13=pass r13;//	if K==1: X`(K)=X(I)!
	if eq jump skp;
		 
	lcntr=K-1,do yyy until lce;
	
		pm(i9,M9)=f4;
		f5=f4+f3; // current Z(i)
		r5= trunc f5;
		f4=f4+f2;//current X`(i)
		
	yyy:dm(i7,m1)=r5;
	
	skp:
	f4=float r0;//X`(K)=X(I)
	pm(i9,M9)=f4;
	
	f5=f4+f3;//Z(K)
	r5=trunc f5;
	
xxx:dm(i7,m1)=r5;
	
	rti(db);
		r14=1;
		r10=r10 xor r14;
//------------------------------------------------------
	
//------------------------------------------------------
//PP for calculate (prepare for upscaling)
//f2 - Step of interpolation,f4 - X`(I)
//f0 = X(i-1) - but return like r0 X(i);
//Rewrite: f2,f0,f4;
calculate:
	f2= float r2;
	f2= f2 * f15;// SI .step of interpolation
		
	f0 = float r0;
	f4= f0+f2;//X`(i)
	
	rts(db);
		r0 = fix f0;
		r0= r1;//X(i-1)=X(i)
//------------------------------------------------------
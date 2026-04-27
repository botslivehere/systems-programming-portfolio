/*************************************************************/
/*Software Engineering for SRV and IV (Part 1) (2022-2023)   */
/*Task 1.0 - Lab-4. Interrupt Handling in real (C FILE)      */
/*Created by Avdzhyan A.E. KTbo2-9  Date:10.Apr.23           */
/*************************************************************/
#include <def21060.h>
#include <signal.h>
#include "const.h"
 
extern void SetupPORTS(int, int,int);

extern int ReadIOPReg(int);

extern void IRQ_Service(int);

//void Duplication(float start_value,float *dst_buffer,int length){
//Function for setting massive 
//by formula dst_buffer[i-1]=i*start_value
void Duplication(float start_value,float *dst_buffer,int length){
	int i;
	for(i=1;length>=i;i++){
		dst_buffer[i-1]=start_value*i;
	}
	return;
}


int main( void )
{
	
    SetupPORTS(1,0xC05F1,DIVISOR); //SPORT0
    SetupPORTS(0,0x0003FEBF, 0x00000100); //LINK 2
    
    //interrupt on SPORT0 (TX0),when it was call to IRQ_Service!
    interrupt(SIG_SPT0I, IRQ_Service);//Activate interrupt only after setup,thx
    while(1);
    return 0;
}


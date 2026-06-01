#include "TimingSet.h"
#include "usart.h"



void Timing_1s()
{

}




void Timing_1ms()
{
	static int cntNum=0;

	BEEP_DrvScan();	
	LED_DrvScan();	
	if(++cntNum > 60000)
	{
		cntNum = 0;
	}
	
}



void Timing_10ms()
{
	KEY_DrvScan();
	KEY_Scan();
}


void Timing_50ms()
{
	
}

void Timing_500ms()
{
	// Check_SysWorkStatus();
}



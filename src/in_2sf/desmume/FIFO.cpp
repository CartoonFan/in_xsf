/*
	Copyright 2006 yopyop
	Copyright 2007 shash
	Copyright 2007-2012 DeSmuME team

	This file is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 2 of the License, or
	(at your option) any later version.

	This file is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with the this software.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <cstring>

#include "FIFO.h"
#include "armcpu.h"
//#include "debug.h"
#include "mem.h"
#include "MMU.h"
#include "NDSSystem.h"
//#include "gfx3d.h"

// ========================================================= IPC FIFO
IPC_FIFO ipc_fifo[2];		// 0 - ARM9
							// 1 - ARM7

void IPC_FIFOinit(uint8_t proc)
{
	memset(&ipc_fifo[proc], 0, sizeof(IPC_FIFO));
	T1WriteWord(MMU.MMU_MEM[proc][0x40], 0x184, 0x00000101);
}

void IPC_FIFOsend(uint8_t proc, uint32_t val)
{
	uint16_t cnt_l = T1ReadWord(MMU.MMU_MEM[proc][0x40], 0x184);
	if (!(cnt_l & IPCFIFOCNT_FIFOENABLE)) return;			// FIFO disabled
	uint8_t	proc_remote = proc ^ 1;

	if (ipc_fifo[proc].size > 15)
	{
		cnt_l |= IPCFIFOCNT_FIFOERROR;
		T1WriteWord(MMU.MMU_MEM[proc][0x40], 0x184, cnt_l);
		return;
	}

	uint16_t cnt_r = T1ReadWord(MMU.MMU_MEM[proc_remote][0x40], 0x184);

	//LOG("IPC%s send FIFO 0x%08X size %03i (l 0x%X, tail %02i) (r 0x%X, tail %02i)\n",
	//	proc?"7":"9", val, ipc_fifo[proc].size, cnt_l, ipc_fifo[proc].tail, cnt_r, ipc_fifo[proc^1].tail);

	cnt_l &= 0xBFFC;		// clear send empty bit & full
	cnt_r &= 0xBCFF;		// set recv empty bit & full
	ipc_fifo[proc].buf[ipc_fifo[proc].tail] = val;
	ipc_fifo[proc].tail++;
	ipc_fifo[proc].size++;
	if (ipc_fifo[proc].tail > 15) ipc_fifo[proc].tail = 0;

	if (ipc_fifo[proc].size > 15)
	{
		cnt_l |= IPCFIFOCNT_SENDFULL;		// set send full bit
		cnt_r |= IPCFIFOCNT_RECVFULL;		// set recv full bit
	}

	T1WriteWord(MMU.MMU_MEM[proc][0x40], 0x184, cnt_l);
	T1WriteWord(MMU.MMU_MEM[proc_remote][0x40], 0x184, cnt_r);

	if (cnt_r & IPCFIFOCNT_RECVIRQEN)
		NDS_makeIrq(proc_remote, IRQ_BIT_IPCFIFO_RECVNONEMPTY);

	NDS_Reschedule();
}

uint32_t IPC_FIFOrecv(uint8_t proc)
{
	uint16_t cnt_l = T1ReadWord(MMU.MMU_MEM[proc][0x40], 0x184);
	if (!(cnt_l & IPCFIFOCNT_FIFOENABLE)) return 0;									// FIFO disabled
	uint8_t	proc_remote = proc ^ 1;

	uint32_t val = 0;

	if ( ipc_fifo[proc_remote].size == 0 )		// remote FIFO error
	{
		cnt_l |= IPCFIFOCNT_FIFOERROR;
		T1WriteWord(MMU.MMU_MEM[proc][0x40], 0x184, cnt_l);
		return 0;
	}

	uint16_t cnt_r = T1ReadWord(MMU.MMU_MEM[proc_remote][0x40], 0x184);

	cnt_l &= 0xBCFF;		// clear send full bit & empty
	cnt_r &= 0xBFFC;		// set recv full bit & empty

	val = ipc_fifo[proc_remote].buf[ipc_fifo[proc_remote].head];
	ipc_fifo[proc_remote].head++;
	ipc_fifo[proc_remote].size--;
	if (ipc_fifo[proc_remote].head > 15) ipc_fifo[proc_remote].head = 0;

	//LOG("IPC%s recv FIFO 0x%08X size %03i (l 0x%X, tail %02i) (r 0x%X, tail %02i)\n",
	//	proc?"7":"9", val, ipc_fifo[proc].size, cnt_l, ipc_fifo[proc].tail, cnt_r, ipc_fifo[proc^1].tail);


	if ( ipc_fifo[proc_remote].size == 0 )		// FIFO empty
	{
		cnt_l |= IPCFIFOCNT_RECVEMPTY;
		cnt_r |= IPCFIFOCNT_SENDEMPTY;

		if (cnt_r & IPCFIFOCNT_SENDIRQEN)
			NDS_makeIrq(proc_remote, IRQ_BIT_IPCFIFO_SENDEMPTY);
	}

	T1WriteWord(MMU.MMU_MEM[proc][0x40], 0x184, cnt_l);
	T1WriteWord(MMU.MMU_MEM[proc_remote][0x40], 0x184, cnt_r);

	NDS_Reschedule();

	return val;
}

void IPC_FIFOcnt(uint8_t proc, uint16_t val)
{
	uint16_t cnt_l = T1ReadWord(MMU.MMU_MEM[proc][0x40], 0x184);
	uint16_t cnt_r = T1ReadWord(MMU.MMU_MEM[proc^1][0x40], 0x184);

	if (val & IPCFIFOCNT_FIFOERROR)
	{
		//at least SPP uses this, maybe every retail game
		cnt_l &= ~IPCFIFOCNT_FIFOERROR;
	}

	if (val & IPCFIFOCNT_SENDCLEAR)
	{
		ipc_fifo[proc].head = 0; ipc_fifo[proc].tail = 0; ipc_fifo[proc].size = 0;

		cnt_l |= IPCFIFOCNT_SENDEMPTY;
		cnt_r |= IPCFIFOCNT_RECVEMPTY;

		cnt_l &= ~IPCFIFOCNT_SENDFULL;
		cnt_r &= ~IPCFIFOCNT_RECVFULL;
	}

	cnt_l &= ~IPCFIFOCNT_WRITEABLE;
	cnt_l |= val & IPCFIFOCNT_WRITEABLE;

	// IPCFIFOCNT_SENDIRQEN may have been set (and/or the fifo may have been cleared) so we may need to trigger this irq
	// (this approach is used by libnds fifo system on occasion in fifoInternalSend, and began happening frequently for value32 with r4326)
	if (cnt_l & IPCFIFOCNT_SENDIRQEN)
		if (cnt_l & IPCFIFOCNT_SENDEMPTY)
			NDS_makeIrq(proc, IRQ_BIT_IPCFIFO_SENDEMPTY);

	// IPCFIFOCNT_RECVIRQEN may have been set so we may need to trigger this irq
	if(cnt_l & IPCFIFOCNT_RECVIRQEN)
		if (!(cnt_l & IPCFIFOCNT_RECVEMPTY))
			NDS_makeIrq(proc, IRQ_BIT_IPCFIFO_RECVNONEMPTY);

	T1WriteWord(MMU.MMU_MEM[proc][0x40], 0x184, cnt_l);
	T1WriteWord(MMU.MMU_MEM[proc^1][0x40], 0x184, cnt_r);

	NDS_Reschedule();
}

// ========================================================= GFX FIFO
//GFX_PIPE	gxPIPE;
//GFX_FIFO	gxFIFO;

/*void GFX_PIPEclear()
{
	gxPIPE.head = 0;
	gxPIPE.tail = 0;
	gxPIPE.size = 0;
}*/

/*void GFX_FIFOclear()
{
	gxFIFO.head = 0;
	gxFIFO.tail = 0;
	gxFIFO.size = 0;
}*/

/*static void GXF_FIFO_handleEvents()
{
	bool low = gxFIFO.size <= 127;
	bool lowchange = MMU_new.gxstat.fifo_low ^ low;
	MMU_new.gxstat.fifo_low = low;
	if(low) triggerDma(EDMAMode_GXFifo);

	bool empty = gxFIFO.size == 0;
	bool emptychange = MMU_new.gxstat.fifo_empty ^ empty;
	MMU_new.gxstat.fifo_empty = empty;

	if(emptychange||lowchange) NDS_Reschedule();
}*/

/*void GFX_FIFOsend(uint8_t cmd, uint32_t param)
{
	if(cmd==0x41) {
		//int zzz=9;
	}

	//INFO("gxFIFO: send 0x%02X = 0x%08X (size %03i/0x%02X) gxstat 0x%08X\n", cmd, param, gxFIFO.size, gxFIFO.size, gxstat);
	//printf("fifo recv: %02X: %08X upto:%d\n",cmd,param,gxFIFO.size+1);

	//TODO - WOAH ! NOT HANDLING A TOO-BIG FIFO RIGHT NOW!
	//if (gxFIFO.size > 255)
	//{
	//	GXF_FIFO_handleEvents();
	//	//NEED TO HANDLE THIS!!!!!!!!!!!!!!!!!!!!!!!!!!

	//	//gxstat |= 0x08000000;			// busy
	//	NDS_RescheduleGXFIFO(1);
	//	//INFO("ERROR: gxFIFO is full (cmd 0x%02X = 0x%08X) (prev cmd 0x%02X = 0x%08X)\n", cmd, param, gxFIFO.cmd[255], gxFIFO.param[255]);
	//	return;
	//}


	gxFIFO.cmd[gxFIFO.tail] = cmd;
	gxFIFO.param[gxFIFO.tail] = param;
	gxFIFO.tail++;
	gxFIFO.size++;
	if (gxFIFO.tail > HACK_GXIFO_SIZE-1) gxFIFO.tail = 0;

	if(gxFIFO.size>=HACK_GXIFO_SIZE) {
		printf("--FIFO FULL-- : %d\n",gxFIFO.size);
	}

	//gxstat |= 0x08000000;		// set busy flag

	GXF_FIFO_handleEvents();

	NDS_RescheduleGXFIFO(1);
}*/

// this function used ONLY in gxFIFO
/*bool GFX_PIPErecv(uint8_t *cmd, uint32_t *param)
{
	//gxstat &= 0xF7FFFFFF;		// clear busy flag

	if (gxFIFO.size == 0)
	{
		GXF_FIFO_handleEvents();
		return false;
	}

	*cmd = gxFIFO.cmd[gxFIFO.head];
	*param = gxFIFO.param[gxFIFO.head];

	gxFIFO.head++;
	gxFIFO.size--;
	if (gxFIFO.head > HACK_GXIFO_SIZE-1) gxFIFO.head = 0;

	GXF_FIFO_handleEvents();

	return true;
}*/

/*void GFX_FIFOcnt(uint32_t val)
{
	////INFO("gxFIFO: write cnt 0x%08X (prev 0x%08X) FIFO size %03i PIPE size %03i\n", val, gxstat, gxFIFO.size, gxPIPE.size);

	if (val & (1<<29))		// clear? (only in homebrew?)
	{
		GFX_PIPEclear();
		GFX_FIFOclear();
		return;
	}

	//zeromus says: what happened to clear stack?
	//if (val & (1<<15))		// projection stack pointer reset
	//{
	//	gfx3d_ClearStack();
	//	val &= 0xFFFF5FFF;		// clear reset (bit15) & stack level (bit13)
	//}

	T1WriteLong(MMU.MMU_MEM[ARMCPU_ARM9][0x40], 0x600, val);
}*/

// ========================================================= DISP FIFO
//DISP_FIFO	disp_fifo;

/*void DISP_FIFOinit()
{
	memset(&disp_fifo, 0, sizeof(DISP_FIFO));
}*/

/*void DISP_FIFOsend(uint32_t val)
{
	//INFO("DISP_FIFO send value 0x%08X (head 0x%06X, tail 0x%06X)\n", val, disp_fifo.head, disp_fifo.tail);
	disp_fifo.buf[disp_fifo.tail] = val;
	disp_fifo.tail++;
	if (disp_fifo.tail > 0x5FFF)
		disp_fifo.tail = 0;
}*/

/*uint32_t DISP_FIFOrecv()
{
	//if (disp_fifo.tail == disp_fifo.head) return 0; // FIFO is empty
	uint32_t val = disp_fifo.buf[disp_fifo.head];
	disp_fifo.head++;
	if (disp_fifo.head > 0x5FFF)
		disp_fifo.head = 0;
	return val;
}*/
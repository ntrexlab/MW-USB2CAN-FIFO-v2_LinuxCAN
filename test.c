#include <iostream>
#include <thread>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include "CAN_Access.h"

	void SendCanMessage(CAN_HANDLE h)
{
	long sid = 0;

	unsigned char sdata[8] = { 0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,0x00,0x11 };

	int ret1 = CAN_Send(h, sid, 8, (char*)sdata, 0, 0);

	if (!ret1)
	{
		printf("Send failed.\n");
	}
}

void RecvCanMessage(CAN_HANDLE h)
{
	char rdata[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	long rid;
	int rlen, ext, rtr;

	int ret = _CAN_Recv(h, &rid, &rlen, rdata, &ext, &rtr);

	if (ret) {

		printf("Recv: [%ld] %02X %02X %02X %02X %02X %02X %02X %02X (%d)\n", rid, (int)(unsigned char)rdata[0], (int)(unsigned char)rdata[1],
			(int)(unsigned char)rdata[2], (int)(unsigned char)rdata[3],
			(int)(unsigned char)rdata[4], (int)(unsigned char)rdata[5],
			(int)(unsigned char)rdata[6], (int)(unsigned char)rdata[7],
			rlen);
	}
}

using namespace std;
sem_t hEvent;
int isFree = 0;
CAN_HANDLE h = 0;
int recvCan()
{
	struct timespec ts;
	int s;

	if (sem_init(&hEvent, 0, 0) == -1)
	{
		printf("sem_init return -1\n");
		return -1;
	}

	CAN_SetRxEventNotification(h, &hEvent);

	int status = CAN_SetConfig(h, 250, 0, 0, 1, 1);
	if (status == 0) {
		printf("CAN_SetConfig return 0\n");
		return -2;
	}

	CAN_SetTransferMode(h, 1);

	sleep(1);

	while (true)
	{
		SendCanMessage(h);
		int rx_count = CAN_CountRxQueue(h);

		printf("%d\n", rx_count);
		if (rx_count > 0)
		{
			RecvCanMessage(h);
		}
		else
		{
			clock_gettime(CLOCK_REALTIME, &ts);
			ts.tv_sec += 1;
			while ((s = sem_timedwait(&hEvent, &ts)) == -1 && errno == EINTR) {
				continue;
			}
			if (s == -1) {
				if (errno == ETIMEDOUT)
					printf("sem_timedwait() timed out\n");
				else
					perror("sem_timedwait");
			}
			else
				printf("sem_timedwait() succeded\n");
		}
		if (isFree)
			break;

	}
	printf("call1\n");
	CAN_Close(h);
	printf("call2\n");

}
int main(int, char**)
{
	int noDevice = CAN_Fifo_ScanSerialNumber();

	if (noDevice <= 0)
	{
		printf("eror noDevice %d\n", noDevice);
		return -1;
	}

	for (int i = 0; i < noDevice; i++)
	{
		const char* SerialNumber = CAN_Fifo_GetSerialNumber(i);

		printf("%s\n", SerialNumber);
	}

	h = CAN_OpenFifo(CAN_Fifo_GetSerialNumber(0));

	if (h < 0)
	{
		printf("USB2CAN open failed. \n");
		return -1;
	}

	thread rThread(recvCan);

	for (int i = 0; i < 10000; i++)
	{
		sleep(1);
	}
	isFree = 1;

	rThread.join();
	printf("wait...");

	return 0;
}
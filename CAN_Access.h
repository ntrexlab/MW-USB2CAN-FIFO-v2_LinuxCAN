#define CAN_HANDLE		long

long CAN_OpenFifo(const char *serialNumber);
int  CAN_Close(CAN_HANDLE handle);

int  CAN_IsOpened (CAN_HANDLE handle);
int  CAN_CountRxQueue (CAN_HANDLE handle);
void CAN_Purge (CAN_HANDLE handle);

int _CAN_Recv (CAN_HANDLE handle, long *ID, int *length, char data[8], int *Ext, int *RTR);
int  CAN_Send (CAN_HANDLE handle, long ID, int length, char data[8], int Ext, int RTR);

int  CAN_GetConfig (CAN_HANDLE handle, long *bitrate, unsigned long *filterID, unsigned long *filterMask, int *startupTransferMode, int *busOffRecovery);
int  CAN_SetConfig (CAN_HANDLE handle, long  bitrate, unsigned long  filterID, unsigned long  filterMask, int  startupTransferMode, int  busOffRecovery);

int  CAN_SetTransferMode (CAN_HANDLE handle, int mode);
int  CAN_SetRxEventNotification (CAN_HANDLE handle, sem_t *hEvent);

int   CAN_Fifo_ScanSerialNumber ();
char* CAN_Fifo_GetSerialNumber (int index);

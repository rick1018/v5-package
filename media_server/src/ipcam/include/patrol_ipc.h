// patrol_ipc.h
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define PT_S_MSG_KEY	0x1234
#define PT_C_MSG_KEY	0x5678

#define IPCBUFSIZE	512

#define IPC_SUCCESS	0
#define	IPC_FAILED	-1

int initIPCInterface(key_t key);
int IPCSendCommand(int msqid, char* buf, size_t buf_len);
int IPCRecvCommand(int msqid, char* buf, size_t* buf_len);
int freeIPCInterface(int msqid);

// end of patrol_ipc.h

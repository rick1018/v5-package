/*
 * pelcod_util.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <pelcod_util.h>
#define GIO_RS485	88//by frank added, for control board issue
#define DEV_NAME	"/dev/tts/0"
#define GIO_GPI_HIGH   0
#define GIO_GPI_LOW    1
int RS485_Setup(int baudrate, int databits, int parity, int stopbit)
{
	struct termios options;
	int fd;
	int br = 0;

	int speed_arr[8][2] = { {B115200, BR115200},
							{B57600, BR57600},
							{B38400, BR38400},
							{B19200, BR19200},
							{B9600, BR9600},
							{B4800, BR4800},
							{B2400, BR2400},
							{B1200, BR1200}};
	
	/* set configuration*/
	fd = open(DEV_NAME, O_RDWR | O_NOCTTY | O_NONBLOCK);
	if (fd < 0)
		return -1;

	do {
		if (tcgetattr(fd, &options) != 0)
			break;
		tcflush(fd, TCIOFLUSH);

		/* set baudrate */
		for ( ; br < 8; br++)
		{
			if (baudrate == speed_arr[br][1])
			{
				cfsetispeed(&options, speed_arr[br][0]);
				cfsetospeed(&options, speed_arr[br][0]);
				break;
			}
		}
		if (br >= 8)
			break;

		options.c_iflag = IGNPAR | ICRNL;
		options.c_oflag = 0;
		options.c_lflag = 0;

		/* set databits */
		options.c_cflag &= ~CSIZE;
		if (databits == DATABITS_7)
			options.c_cflag |= CS7;
		else if (databits == DATABITS_8)
			options.c_cflag |= CS8;
		else
			break;

		/* set parity */
		if (parity == PARITY_N)
		{
			options.c_cflag &= ~PARENB;
			options.c_iflag &= ~INPCK;
		}
		else if (parity == PARITY_O)
		{
			options.c_cflag |= (PARODD | PARENB);
			options.c_iflag |= INPCK;
		}
		else if (parity == PARITY_E)
		{
			options.c_cflag |= PARENB;
			options.c_cflag &= ~PARODD;
			options.c_iflag |= INPCK;
		}
		else if (parity == PARITY_S)
		{
			options.c_cflag &= ~PARENB;
			options.c_cflag &= ~CSTOPB;
		}
		else
			break;

		/* set stopbit */
		if (stopbit == STOPBIT_1)
			options.c_cflag &= ~CSTOPB;
		else if (stopbit == STOPBIT_2)
			options.c_cflag |= CSTOPB;
		else
			break;
			
		if (parity != PARITY_N)
			options.c_iflag |= INPCK;

		options.c_cc[VTIME] = 0;
		options.c_cc[VMIN] = 1;

		tcflush(fd, TCIFLUSH);
		if (tcsetattr(fd, TCSANOW, &options) != 0)
			break;

		close(fd);
		return 0;
	} while(0);

	close(fd);
	return -1;
}
#if 0  // remark by kenny chiu 20130521
int  CheckTxRxData()
{
	char szSend[] = "SMAX";
	char szRecv[6];
	int iSend, iRecv;
	int fd = open(DEV_NAME, O_RDWR);
	if (fd < 0)
		return -1;
	fprintf(stderr, "CheckTxRxData start\n");
	dm355_gio_write(GIO_RS485, GIO_GPI_LOW);//by frank added
	iSend = write(fd, szSend, strlen(szSend));
//	dm355_gio_write(GIO_RS485, GIO_GPI_HIGH);//by frank added
	sleep(1);
//	dm355_gio_write(GIO_RS485, GIO_GPI_LOW);//by frank added
	iRecv = read(fd, (char*)szRecv, strlen(szSend));
	dm355_gio_write(GIO_RS485, GIO_GPI_HIGH);//by frank added
	fprintf(stderr, "CheckTxRxData iSend = %d, iRecv = %d, szRecv = %s\n", iSend, iRecv, szRecv);
	close(fd);
	if (strcmp(szSend, szRecv) == 0)
		return 1;
	return -1;
}
#endif
char CheckSum(char* strCommOut)
{
	int sum = 0;
	int i = 1;

	for ( ; i < 6; i++)
		sum += strCommOut[i];

	return sum & 0xFF;
}

int SendCommand(char* cmd)
{
//	int i;
	dm355_gio_write(GIO_RS485, GIO_GPI_LOW);//by frank added
	int fd = open(DEV_NAME, O_RDWR | O_NOCTTY | O_NONBLOCK);
	if (fd < 0)
		return -1;
	write(fd, cmd, 7);
	close(fd);
	dm355_gio_write(GIO_RS485, GIO_GPI_HIGH);//by frank added
	fprintf(stderr, "Pelco-D: %02x %02x %02x %02x %02x %02x %02x\n", cmd[0], cmd[1], cmd[2], cmd[3], cmd[4], cmd[5], cmd[6]);
	return 0;
}

int Pelco_D_AutoPan(char camera_id)
{
	char strCommOut[7];

	strCommOut[0] = 0xFF;
	strCommOut[1] = camera_id & 0x3f;
	strCommOut[2] = 0x98;
	strCommOut[3] = 0x0b;
	strCommOut[4] = 0x03;
	strCommOut[5] = 0x03;
	strCommOut[6] = CheckSum(strCommOut);

	return SendCommand(strCommOut);
}

int Pelco_D_CamLight(char camera_id)
{
	char strCommOut[7];

	strCommOut[0] = 0xFF;
	strCommOut[1] = camera_id & 0x3f;
	strCommOut[2] = 0x00;
	strCommOut[3] = 0x09;
	strCommOut[4] = 0x00;
	strCommOut[5] = 0x01;
	strCommOut[6] = CheckSum(strCommOut);

	return SendCommand(strCommOut);
}

int Pelco_D_ClearPreset(char camera_id, char preset_no)
{
	char strCommOut[7];

	strCommOut[0] = 0xFF;
	strCommOut[1] = camera_id & 0x3f;
	strCommOut[2] = 0x00;
	strCommOut[3] = 0x05;
	strCommOut[4] = 0x00;
	strCommOut[5] = preset_no;
	strCommOut[6] = CheckSum(strCommOut);

	return SendCommand(strCommOut);
}

int Pelco_D_DownLeft(char camera_id, char p_speed, char t_speed)
{
	char strCommOut[7];

	strCommOut[0] = 0xFF;
	strCommOut[1] = camera_id & 0x3f;
	strCommOut[2] = 0x00;
	strCommOut[3] = 0x14;
	strCommOut[4] = p_speed;
	strCommOut[5] = t_speed;
	strCommOut[6] = CheckSum(strCommOut);

	return SendCommand(strCommOut);
}

int Pelco_D_DownRight(char camera_id, char p_speed, char t_speed)
{
	char strCommOut[7];

	strCommOut[0] = 0xFF;
	strCommOut[1] = camera_id & 0x3f;
	strCommOut[2] = 0x00;
	strCommOut[3] = 0x12;
	strCommOut[4] = p_speed;
	strCommOut[5] = t_speed;
	strCommOut[6] = CheckSum(strCommOut);

	return SendCommand(strCommOut);
}

int Pelco_D_FocusFar(char camera_id)
{
	char strCommOut[7];

	strCommOut[0] = 0xFF;
	strCommOut[1] = camera_id & 0x3f;
	strCommOut[2] = 0x00;
	strCommOut[3] = 0x80;
	strCommOut[4] = 0x00;
	strCommOut[5] = 0x00;
	strCommOut[6] = CheckSum(strCommOut);

	return SendCommand(strCommOut);
}

int Pelco_D_FocusNear(char camera_id)
{
	char strCommOut[7];

	strCommOut[0] = 0xFF;
	strCommOut[1] = camera_id & 0x3f;
	strCommOut[2] = 0x01;
	strCommOut[3] = 0x00;
	strCommOut[4] = 0x00;
	strCommOut[5] = 0x00;
	strCommOut[6] = CheckSum(strCommOut);

	return SendCommand(strCommOut);
}

int Pelco_D_GotoPreset(char camera_id, char preset_id)
{
	char strCommOut[7];

	strCommOut[0] = 0xFF;
	strCommOut[1] = camera_id & 0x3f;
	strCommOut[2] = 0x00;
	strCommOut[3] = 0x07;
	strCommOut[4] = 0x00;
	strCommOut[5] = preset_id;
	strCommOut[6] = CheckSum(strCommOut);

	return SendCommand(strCommOut);
}

int Pelco_D_IrisClose(char camera_id)
{
	char strCommOut[7];

	strCommOut[0] = 0xFF;
	strCommOut[1] = camera_id & 0x3f;
	strCommOut[2] = 0x04;
	strCommOut[3] = 0x00;
	strCommOut[4] = 0x00;
	strCommOut[5] = 0x00;
	strCommOut[6] = CheckSum(strCommOut);

	return SendCommand(strCommOut);
}

int Pelco_D_IrisOpen(char camera_id)
{
	char strCommOut[7];

	strCommOut[0] = 0xFF;
	strCommOut[1] = camera_id & 0x3f;
	strCommOut[2] = 0x02;
	strCommOut[3] = 0x00;
	strCommOut[4] = 0x00;
	strCommOut[5] = 0x00;
	strCommOut[6] = CheckSum(strCommOut);

	return SendCommand(strCommOut);
}

int Pelco_D_PanLeft(char camera_id, char p_speed)
{
	char strCommOut[7];

	strCommOut[0] = 0xFF;
	strCommOut[1] = camera_id & 0x3f;
	strCommOut[2] = 0x00;
	strCommOut[3] = 0x04;
	strCommOut[4] = p_speed;
	strCommOut[5] = 0x00;
	strCommOut[6] = CheckSum(strCommOut);

	return SendCommand(strCommOut);
}

int Pelco_D_PanRight(char camera_id, char p_speed)
{
	char strCommOut[7];

	strCommOut[0] = 0xFF;
	strCommOut[1] = camera_id & 0x3f;
	strCommOut[2] = 0x00;
	strCommOut[3] = 0x02;
	strCommOut[4] = p_speed;
	strCommOut[5] = 0x00;
	strCommOut[6] = CheckSum(strCommOut);

	return SendCommand(strCommOut);
}

int Pelco_D_PatternRun(char camera_id)
{
	char strCommOut[7];

	strCommOut[0] = 0xFF;
	strCommOut[1] = camera_id & 0x3f;
	strCommOut[2] = 0x00;
	strCommOut[3] = 0x23;
	strCommOut[4] = 0x00;
	strCommOut[5] = 0x00;
	strCommOut[6] = CheckSum(strCommOut);

	return SendCommand(strCommOut);
}

int Pelco_D_PatternStart(char camera_id)
{
	char strCommOut[7];

	strCommOut[0] = 0xFF;
	strCommOut[1] = camera_id & 0x3f;
	strCommOut[2] = 0x00;
	strCommOut[3] = 0x1F;
	strCommOut[4] = 0x00;
	strCommOut[5] = 0x00;
	strCommOut[6] = CheckSum(strCommOut);

	return SendCommand(strCommOut);
}

int Pelco_D_PatternStop(char camera_id)
{
	char strCommOut[7];

	strCommOut[0] = 0xFF;
	strCommOut[1] = camera_id & 0x3f;
	strCommOut[2] = 0x00;
	strCommOut[3] = 0x21;
	strCommOut[4] = 0x00;
	strCommOut[5] = 0x00;
	strCommOut[6] = CheckSum(strCommOut);

	return SendCommand(strCommOut);
}

int Pelco_D_Stop(char camera_id)
{
	char strCommOut[7];

	strCommOut[0] = 0xFF;
	strCommOut[1] = camera_id & 0x3f;
	strCommOut[2] = 0x00;
	strCommOut[3] = 0x00;
	strCommOut[4] = 0x00;
	strCommOut[5] = 0x00;
	strCommOut[6] = CheckSum(strCommOut);

	return SendCommand(strCommOut);
}

int Pelco_D_FocusSpeed(char camera_id, char speed)
{
	char strCommOut[7];

	strCommOut[0] = 0xFF;
	strCommOut[1] = camera_id & 0x3f;
	strCommOut[2] = 0x00;
	strCommOut[3] = 0x27;
	strCommOut[4] = 0x00;
	strCommOut[5] = speed;
	strCommOut[6] = CheckSum(strCommOut);

	return SendCommand(strCommOut);
}

int Pelco_D_SetPreset(char camera_id, char preset_no)
{
	char strCommOut[7];

	strCommOut[0] = 0xFF;
	strCommOut[1] = camera_id & 0x3f;
	strCommOut[2] = 0x00;
	strCommOut[3] = 0x03;
	strCommOut[4] = 0x00;
	strCommOut[5] = preset_no;
	strCommOut[6] = CheckSum(strCommOut);

	return SendCommand(strCommOut);
}

int Pelco_D_ZoomSpeed(char camera_id, char speed)
{
	char strCommOut[7];

	strCommOut[0] = 0xFF;
	strCommOut[1] = camera_id & 0x3f;
	strCommOut[2] = 0x00;
	strCommOut[3] = 0x25;
	strCommOut[4] = 0x00;
	strCommOut[5] = speed;
	strCommOut[6] = CheckSum(strCommOut);

	return SendCommand(strCommOut);
}

int Pelco_D_TiltDown(char camera_id, char t_speed)
{
	char strCommOut[7];

	strCommOut[0] = 0xFF;
	strCommOut[1] = camera_id & 0x3f;
	strCommOut[2] = 0x00;
	strCommOut[3] = 0x10;
	strCommOut[4] = 0x00;
	strCommOut[5] = t_speed;
	strCommOut[6] = CheckSum(strCommOut);

	return SendCommand(strCommOut);
}

int Pelco_D_TiltUp(char camera_id, char t_speed)
{
	char strCommOut[7];

	strCommOut[0] = 0xFF;
	strCommOut[1] = camera_id & 0x3f;
	strCommOut[2] = 0x00;
	strCommOut[3] = 0x08;
	strCommOut[4] = 0x00;
	strCommOut[5] = t_speed;
	strCommOut[6] = CheckSum(strCommOut);

	return SendCommand(strCommOut);
}

int Pelco_D_UpLeft(char camera_id, char p_speed, char t_speed)
{
	char strCommOut[7];

	strCommOut[0] = 0xFF;
	strCommOut[1] = camera_id & 0x3f;
	strCommOut[2] = 0x00;
	strCommOut[3] = 0x0C;
	strCommOut[4] = p_speed;
	strCommOut[5] = t_speed;
	strCommOut[6] = CheckSum(strCommOut);

	return SendCommand(strCommOut);
}

int Pelco_D_UpRight(char camera_id, char p_speed, char t_speed)
{
	char strCommOut[7];

	strCommOut[0] = 0xFF;
	strCommOut[1] = camera_id & 0x3f;
	strCommOut[2] = 0x00;
	strCommOut[3] = 0x0A;
	strCommOut[4] = p_speed;
	strCommOut[5] = t_speed;
	strCommOut[6] = CheckSum(strCommOut);

	return SendCommand(strCommOut);
}

int Pelco_D_ZoomIn(char camera_id)
{
	char strCommOut[7];

	strCommOut[0] = 0xFF;
	strCommOut[1] = camera_id & 0x3f;
	strCommOut[2] = 0x00;
	strCommOut[3] = 0x20;
	strCommOut[4] = 0x00;
	strCommOut[5] = 0x00;
	strCommOut[6] = CheckSum(strCommOut);

	return SendCommand(strCommOut);
}

int Pelco_D_ZoomOut(char camera_id)
{
	char strCommOut[7];

	strCommOut[0] = 0xFF;
	strCommOut[1] = camera_id & 0x3f;
	strCommOut[2] = 0x00;
	strCommOut[3] = 0x40;
	strCommOut[4] = 0x00;
	strCommOut[5] = 0x00;
	strCommOut[6] = CheckSum(strCommOut);

	return SendCommand(strCommOut);
}

// end of pelcod_util.c


#ifndef __SHAREDMEMORY__
#define __SHAREDMEMORY__

#define IPC_GPS_KEY ((key_t)5264)
#define IPC_UART_KEY ((key_t)5364)
#define SHARE_MEMORY_GPS_FILE "/tmp/.share_gps_file"
#define SHARE_MEMORY_UART_FILE "/tmp/.share_aurt_file"
#define SHARE_GPS_SIZE sizeof(GPRMC_INFO)
#define SHARE_UART_SIZE 128*sizeof(char)

typedef struct GPGGAInfo {
    char UTCTime[10];
    float latitude;
    char latitudeLocate;
    float longitude;
    char longitudeLocate;
    int status;
    int satelliteCount;
    char HDOP[3];
    float altitude;
    float heightOfGeoid;
    int DGPS;
    char DGPSStationId[3];
    char checksumData[3];
}GPGGA_INFO;

typedef struct GPRMCInfo {
    char UTCTime[10];
    char status;
    double latitude;
    char latitudeLocate;
    double longtitude;
    char longtitudeLocate;
    double speed;
    double azimuth_angle;
    char UTCDate[10];
    double magneticVariation;
    char magneticVariationLocate;
    char modeAndCkecksum[3];
}GPRMC_INFO;

typedef struct GPVTGInfo {
}GPVTG_INFO;
typedef struct GPGSAInfo {
}GPGSA_INFO;
typedef struct GPGSVInfo {
}GPGSV_INFO;

typedef struct gpsInfo {
    GPGGA_INFO *gpgga;
    GPRMC_INFO gprmc;
    GPVTG_INFO gpvtg;
    GPGSA_INFO gpgsa;
    GPGSV_INFO gpgsv;
}GPS_INFO;

typedef enum{
    UART = 0,
    GPS
}SHARE_TYPE;

typedef enum{
    READ = 0,
    WRITE
}SHARE_MODE;

void shmSetInfo(char *data, char *shmadd, SHARE_TYPE type);
char* shmGetInfo(char *shmadd, SHARE_TYPE type);
char* init_share_memory(SHARE_MODE mode, SHARE_TYPE type);
#endif

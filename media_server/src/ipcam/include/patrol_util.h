// patrol_ipc.h
#ifndef PATROL_UTIL_H
#define PATROL_UTIL_H
#define	HOME			0

// charles lin v9 start
#define	MIN_TILT		-2376//-2700//-2070   // -90 degree
#define	MAX_TILT		792//900//690     // 30 degree

#define	MIN_PAN			-16384//-16461  // 177 degree,  -16600 steps = -178 degree //
#define	MAX_PAN			16383//16461   // 177 degree,  16600 steps = 178 degree // 
// end


#define	MIN_SPEED	0
#define	MAX_SPEED	2

typedef enum { // charles lin v2
	PT_MOVE = 0x0,
	PT_MOVETO,
	PT_MOVE_ORG, //Add by Joseph for onvif 2013-03-06
    PT_STOP,
//	PT_SETSPEED,
	PT_STATUS,
	PT_CAL,
	PT_ACTPOS, //alice_modify_onvif_code_2011
	PT_MVStatus, //alice_modify_onvif_code_2011
	HOME_UPDATE,
	HOME_GOTO,
	PRESET_UPDATE,
	PRESET_LIST,
	PRESET_GOTO,
	PATROL_START,
	PATROL_STOP,
	PATROL_PAUSE,
	PATROL_GETSPEED,
	PATROL_SETSPEED,
	PATROL_UPDATE,
	PATROL_LIST,
	PATROL_STATUS,		   // charles lin v6
	//PATROL_GET_DWELL, 
	//PATROL_SET_DWELL,
	LOAD_DEFAULT,
} PTCODE;

/* PT */
typedef struct {
	int	x;
	int	y;
	int zoom;
	int	rotate;
} PTData;

/* Preset */
#define PRESET_ENABLED  1 // charles lin v6
#define PRESET_DISABLED 0
#define MAX_PT_PRESET_NAME_LEN	19
#define MAX_PATROL_NAME_LEN	19


typedef struct { // charles lin v2
	int 	x;
	int 	y;
	int 	zoom;
	int 	rotate;
} HomeEntry;

typedef struct { // charles lin v10
	int 	x;
	int 	y;
	int 	zoom;
	int 	rotate;
} LastEntry;

#define MAX_PRESET_LIST_SIZE	8
#define MAX_PATROL_LIST_SIZE	4 // charles lin v2

typedef struct {
	char	name[MAX_PT_PRESET_NAME_LEN + 1];
	int		enabled;
	int		x;
	int		y;
	int		dwell; // charles lin v6
	int 	zoom; //add by frank for Geo fisheye
	int 	rotate; //add by frank for Geo fisheye, resvered use. now FW spec don't define.
} PresetEntry;

typedef struct {
	char	name[MAX_PATROL_NAME_LEN];
    int		order[MAX_PRESET_LIST_SIZE];
    int 	size;
} PatrolEntry; // charles lin v2

typedef struct {
	PresetEntry entry[MAX_PRESET_LIST_SIZE];
} PresetList;

typedef struct {
    PatrolEntry entry[MAX_PATROL_LIST_SIZE];
} PatrolList; // charles lin v2

#define START_PATROL	1
#define STOP_PATROL		0

#define PT_SUCCESS 0
#define PT_FAILED	-1

// end of PatrolUtil.h

#endif

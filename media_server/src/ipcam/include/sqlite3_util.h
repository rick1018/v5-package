#if defined (__cplusplus)
extern "C" {
#endif

#ifndef __SQLITE3_UTIL_HEADER__
#define __SQLITE3_UTIL_HEADER__
#include <sqlite3.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

#define SQL_CMD_CREATE_TABLE_LOG			"CREATE TABLE IF NOT EXISTS Log( " \
											"id INTEGER PRIMARY KEY" \
											", datetime CHAR(32)" \
											", msg char(64) );"

#define CMD_CREATE_TABLE_AVIREC				"CREATE TABLE IF NOT EXISTS Avirec( " \
											"id INTEGER PRIMARY KEY" \
											", filename CHAR(64)" \
											", filesize INT" \
											", createTime DATETIME" \
											", datetime CHAR(32) );"

#define SQL_CMD_CREATE_TABLE_AVIEVENT		"CREATE TABLE IF NOT EXISTS Avievent( " \
											"id INTEGER PRIMARY KEY" \
											", filename CHAR(64)" \
											", filesize INT" \
											", createTime DATETIME" \
											", datetime CHAR(32) );"

#define SQL_CMD_TOTAL_CNT_QUERY				"SELECT COUNT(*) FROM Log;"
#define SQL_CMD_DELETE_ALL_LOG_QUERY		"DELETE FROM Log;"
#define SQL_CMD_BUFFER_LEN					2048
#define LOG_DB_FILE							"/mnt/nand/log.db"
#define AVI_REC_DB_FILE						"/tmp/mmc_dir/Record/.avirec.db"
#define AVI_EVENT_DB_FILE					"/tmp/mmc_dir/Event/.avievent.db"
#define SDCARD_MEDIA_PATH					"/tmp/mmc_dir"
#define LOG_TABLE_NAME						"Log"
#define AVI_REC_TABLE_NAME					"Avirec"
#define AVI_EVENT_TABLE_NAME				"Avievent"
#define MAX_TOTAL_LEN_OF_LOG_MSG			3072
#define MAX_LEN_OF_LOG_MSG					64
#define MAX_LOG_RECORD						100
#define MAX_AVI_DB_BUFFER_LEN				2048
#define EMPTY_STR							""
#define SQL_LOG_MSG							"/tmp/.sqlite3_util.log"

#define STRING_SQLITE_OK					"Successful result"
#define STRING_SQLITE_ERROR					"SQL error or missing database"
#define STRING_SQLITE_INTERNAL				"Internal logic error in SQLite"
#define STRING_SQLITE_PERM					"Access permission denied"
#define STRING_SQLITE_ABORT					"Callback routine requested an abort"
#define STRING_SQLITE_BUSY					"The database file is locked"
#define STRING_SQLITE_LOCKED				"A table in the database is locked"
#define STRING_SQLITE_NOMEM					"A malloc() failed"
#define STRING_SQLITE_READONLY				"Attempt to write a readonly database"
#define STRING_SQLITE_INTERRUPT				"Operation terminated by sqlite3_interrupt()"
#define STRING_SQLITE_IOERR					"Some kind of disk I/O error occurred"
#define STRING_SQLITE_CORRUPT				"The database disk image is malformed"
#define STRING_SQLITE_NOTFOUND				"Unknown opcode in sqlite3_file_control()"
#define STRING_SQLITE_FULL					"Insertion failed because database is full"
#define STRING_SQLITE_CANTOPEN				"Unable to open the database file"
#define STRING_SQLITE_PROTOCOL				"Database lock protocol error"
#define STRING_SQLITE_EMPTY					"Database is empty"
#define STRING_SQLITE_SCHEMA				"The database schema changed"
#define STRING_SQLITE_TOOBIG				"String or BLOB exceeds size limit"
#define STRING_SQLITE_CONSTRAINT			"Abort due to constraint violation"
#define STRING_SQLITE_MISMATCH				"Data type mismatch"
#define STRING_SQLITE_MISUSE				"Library used incorrectly"
#define STRING_SQLITE_NOLFS					"Uses OS features not supported on host"
#define STRING_SQLITE_AUTH					"Authorization denied"
#define STRING_SQLITE_FORMAT				"Auxiliary database format error"
#define STRING_SQLITE_RANGE					"2nd parameter to sqlite3_bind out of range"
#define STRING_SQLITE_NOTADB				"File opened that is not a database file"
#define STRING_SQLITE_NOTICE				"Notifications from sqlite3_log()"
#define STRING_SQLITE_WARNING				"Warnings from sqlite3_log()"
#define STRING_SQLITE_ROW					"sqlite3_step() has another row ready"
#define STRING_SQLITE_DONE					"sqlite3_step() has finished executing"

int make_sqlite_cmd(char *_cmd_buf, const char* fmt, ...);
int cbfunc_ShowLogList(void *context, int count, char **values, char ** columnName);
int cbfunc_ShowAVIList(void *context, int count, char **values, char ** columnName);
int cbfunc_GetAVIFilename(void *context, int count, char **values, char ** columnName);
int cbfunc_GetListTotal(void *context, int count, char **values, char ** columnName);
int busy_handler(void *data, int retry);
int openLogDb(sqlite3 **_db, char *_db_path);
int createTable(sqlite3 *_db);
int queryLogDB(sqlite3 *_db);
int setLogMsg(char *_msg);
int getLogMsg(int _offset, char *_msg_buf);
int getLogMsgbyIndex(int _index, char *_msg_buf);
int delAllLog(void);
int getLogTotalNum(void);
int getNumOfDBTotalRecord(int _db_type);
void findErrMsg(char *_from_func, int _from_line, int _ret, char *_err_msg, char *_exec_cmd);
int insertAVIRecord2DB(int _rec_media, char *_filename);
int deleteAVIRecord2DB(int _rec_media);
int queryAllAVIDB(int _rec_media, int _page, int _max_record_num_per_page, char *_db_buff);
int queryAllAVIDBByDateTime(int _rec_media, int _page, int _maxNumOfPage, char *_db_buff, struct tm *_start_tm, struct tm *_end_tm);

enum __db_type__{
	ENUM_DB_TYPE_START = 0
	,ENUM_DB_TYPE_IS_LOG
	,ENUM_DB_TYPE_IS_AVIREC
	,ENUM_DB_TYPE_IS_AVIEVENT
};

typedef enum __rec_media__{
	ENUM_REC_MEDIA_START = 0
	,ENUM_REC_MEDIA_IS_AVIREC
	,ENUM_REC_MEDIA_IS_AVIEVENT
}_REC_MEDIA;

typedef struct {
	int max_retry;  /* Max retry times. */
	int sleep_ms;   /* Time to sleep before retry again. */
} busy_handler_attr;
#endif
#if defined (__cplusplus)
}
#endif

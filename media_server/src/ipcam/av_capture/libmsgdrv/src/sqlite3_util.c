#include <sqlite3_util.h>

char g_sql_cmd_buffer[SQL_CMD_BUFFER_LEN];
int g_len_ret1 = 0, g_len_ret2 = 0;

void log_message(char *fmt, ...){
	char buffer[4096];
	struct tm *tm_ptr;
	time_t timeval;
	va_list argptr;

	va_start(argptr, fmt);
	vsprintf(buffer, fmt, argptr);
	va_end(argptr);

	FILE *logfile = fopen(SQL_LOG_MSG, "a");
	if(!logfile)    return;
	(void) time(&timeval);
	tm_ptr = localtime(&timeval);
	fprintf(logfile, "[%04d-%02d-%02d %02d:%02d:%02d][%s] %s\n",
			tm_ptr->tm_year+1900, tm_ptr->tm_mon+1, tm_ptr->tm_mday,
			tm_ptr->tm_hour, tm_ptr->tm_min, tm_ptr->tm_sec, __FILE__, buffer);
	fclose(logfile);
}

void _Sleep(int iSec){
	struct timeval tv;
	tv.tv_sec = iSec;
	tv.tv_usec = 0;
	select(0, NULL, NULL, NULL, &tv);
}

void _Usleep(int iUsec){
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = iUsec;
	select(0, NULL, NULL, NULL, &tv);
}

void findErrMsg(char *_from_func, int _from_line, int _ret, char *_err_msg, char *_exec_cmd){
	int o = 0;
	char buf[1024];
	memset(buf, '\0', sizeof(buf));
	o += sprintf(&buf[o], "%s ", _exec_cmd);
	o += sprintf(&buf[o], "From [%s][%d] ", _from_func, _from_line);
	// fprintf(stderr, "%s ", buf);

	switch(_ret){
		case SQLITE_OK:
			o += sprintf(&buf[o], "%s", STRING_SQLITE_OK);
			// fprintf(stderr, "%s ", buf);
			log_message("%s", buf);
			break;
		case SQLITE_ERROR:
			o += sprintf(&buf[o], "%s : %s", STRING_SQLITE_ERROR, _err_msg);
			// fprintf(stderr, "%s", buf);
			log_message("%s", buf);
			break;
		case SQLITE_INTERNAL:
			o += sprintf(&buf[o], "%s : %s", STRING_SQLITE_INTERNAL, _err_msg);
			// fprintf(stderr, "%s", buf);
			log_message("%s", buf);
			break;
		case SQLITE_PERM:
			o += sprintf(&buf[o], "%s : %s", STRING_SQLITE_PERM, _err_msg);
			// fprintf(stderr, "%s", buf);
			log_message("%s", buf);
			break;
		case SQLITE_ABORT:
			o += sprintf(&buf[o], "%s : %s", STRING_SQLITE_ABORT, _err_msg);
			// fprintf(stderr, "%s", buf);
			log_message("%s", buf);
			break;
		case SQLITE_BUSY:
			o += sprintf(&buf[o], "%s : %s", STRING_SQLITE_BUSY, _err_msg);
			// fprintf(stderr, "%s", buf);
			log_message("%s", buf);
			break;
		case SQLITE_LOCKED:
			o += sprintf(&buf[o], "%s : %s", STRING_SQLITE_LOCKED, _err_msg);
			// fprintf(stderr, "%s", buf);
			log_message("%s", buf);
			break;
		case SQLITE_NOMEM:
			o += sprintf(&buf[o], "%s : %s", STRING_SQLITE_NOMEM, _err_msg);
			// fprintf(stderr, "%s", buf);
			log_message("%s", buf);
			break;
		case SQLITE_READONLY:
			o += sprintf(&buf[o], "%s : %s", STRING_SQLITE_READONLY, _err_msg);
			// fprintf(stderr, "%s", buf);
			log_message("%s", buf);
			break;
		case SQLITE_INTERRUPT:
			o += sprintf(&buf[o], "%s : %s", STRING_SQLITE_INTERRUPT, _err_msg);
			// fprintf(stderr, "%s", buf);
			log_message("%s", buf);
			break;
		case SQLITE_IOERR:
			o += sprintf(&buf[o], "%s : %s", STRING_SQLITE_IOERR, _err_msg);
			// fprintf(stderr, "%s", buf);
			log_message("%s", buf);
			break;
		case SQLITE_CORRUPT:
			o += sprintf(&buf[o], "%s : %s", STRING_SQLITE_CORRUPT, _err_msg);
			// fprintf(stderr, "%s", buf);
			log_message("%s", buf);
			break;
		case SQLITE_NOTFOUND:
			o += sprintf(&buf[o], "%s : %s", STRING_SQLITE_NOTFOUND, _err_msg);
			// fprintf(stderr, "%s", buf);
			log_message("%s", buf);
			break;
		case SQLITE_FULL:
			o += sprintf(&buf[o], "%s : %s", STRING_SQLITE_FULL, _err_msg);
			// fprintf(stderr, "%s", buf);
			log_message("%s", buf);
			break;
		case SQLITE_CANTOPEN:
			o += sprintf(&buf[o], "%s : %s", STRING_SQLITE_CANTOPEN, _err_msg);
			// fprintf(stderr, "%s", buf);
			log_message("%s", buf);
			break;
		case SQLITE_PROTOCOL:
			o += sprintf(&buf[o], "%s : %s", STRING_SQLITE_PROTOCOL, _err_msg);
			// fprintf(stderr, "%s", buf);
			log_message("%s", buf);
			break;
		case SQLITE_EMPTY:
			o += sprintf(&buf[o], "%s : %s", STRING_SQLITE_EMPTY, _err_msg);
			// fprintf(stderr, "%s", buf);
			log_message("%s", buf);
			break;
		case SQLITE_SCHEMA:
			o += sprintf(&buf[o], "%s : %s", STRING_SQLITE_SCHEMA, _err_msg);
			// fprintf(stderr, "%s", buf);
			log_message("%s", buf);
			break;
		case SQLITE_TOOBIG:
			o += sprintf(&buf[o], "%s : %s", STRING_SQLITE_TOOBIG, _err_msg);
			// fprintf(stderr, "%s", buf);
			log_message("%s", buf);
			break;
		case SQLITE_CONSTRAINT:
			o += sprintf(&buf[o], "%s : %s", STRING_SQLITE_CONSTRAINT, _err_msg);
			// fprintf(stderr, "%s", buf);
			log_message("%s", buf);
			break;
		case SQLITE_MISMATCH:
			o += sprintf(&buf[o], "%s : %s", STRING_SQLITE_MISMATCH, _err_msg);
			// fprintf(stderr, "%s", buf);
			log_message("%s", buf);
			break;
		case SQLITE_MISUSE:
			o += sprintf(&buf[o], "%s : %s", STRING_SQLITE_MISUSE, _err_msg);
			// fprintf(stderr, "%s", buf);
			log_message("%s", buf);
			break;
		case SQLITE_NOLFS:
			o += sprintf(&buf[o], "%s : %s", STRING_SQLITE_NOLFS, _err_msg);
			// fprintf(stderr, "%s", buf);
			log_message("%s", buf);
			break;
		case SQLITE_AUTH:
			o += sprintf(&buf[o], "%s : %s", STRING_SQLITE_AUTH, _err_msg);
			// fprintf(stderr, "%s", buf);
			log_message("%s", buf);
			break;
		case SQLITE_FORMAT:
			o += sprintf(&buf[o], "%s : %s", STRING_SQLITE_FORMAT, _err_msg);
			// fprintf(stderr, "%s", buf);
			log_message("%s", buf);
			break;
		case SQLITE_RANGE:
			o += sprintf(&buf[o], "%s : %s", STRING_SQLITE_RANGE, _err_msg);
			// fprintf(stderr, "%s", buf);
			log_message("%s", buf);
			break;
		case SQLITE_NOTADB:
			o += sprintf(&buf[o], "%s : %s", STRING_SQLITE_NOTADB, _err_msg);
			// fprintf(stderr, "%s", buf);
			log_message("%s", buf);
			break;
		case SQLITE_NOTICE:
			o += sprintf(&buf[o], "%s : %s", STRING_SQLITE_NOTICE, _err_msg);
			// fprintf(stderr, "%s", buf);
			log_message("%s", buf);
			break;
		case SQLITE_WARNING:
			o += sprintf(&buf[o], "%s : %s", STRING_SQLITE_WARNING, _err_msg);
			// fprintf(stderr, "%s", buf);
			log_message("%s", buf);
			break;
		case SQLITE_ROW:
			o += sprintf(&buf[o], "%s : %s", STRING_SQLITE_ROW, _err_msg);
			// fprintf(stderr, "%s", buf);
			log_message("%s", buf);
			break;
		case SQLITE_DONE:
			o += sprintf(&buf[o], "%s : %s", STRING_SQLITE_DONE, _err_msg);
			// fprintf(stderr, "%s", buf);
			log_message("%s", buf);
			break;
		default:
			o += sprintf(&buf[o], "Unknown %d", _ret);
			// fprintf(stderr, "%s", buf);
			log_message("%s", buf);
			break;
	};
	// fprintf(stderr, "\n");
}

int make_sqlite_cmd(char *_cmd_buf, const char* fmt, ...){
	int o = 0;
	va_list va;

	va_start(va, fmt);
	memset(_cmd_buf, '\0', SQL_CMD_BUFFER_LEN);
	o += vsprintf(&_cmd_buf[o], fmt, va);
	return o;
}

int cbfunc_ShowLogList(void *context, int count, char **values, char ** columnName){
	// sprintf((char*)context, "%s %s", values[0], values[1]);
	g_len_ret2 += sprintf((char*)&context[g_len_ret2], "%s %s|", values[0], values[1]);
	return SQLITE_OK;
}

int cbfunc_ShowAVIList(void *context, int count, char **values, char ** columnName){
	g_len_ret1 += sprintf((char*)&context[g_len_ret1], "%s|%s|%s,", values[0], values[1], values[2]);
	return SQLITE_OK;
}

int cbfunc_GetAVIID(void *context, int count, char **values, char ** columnName){
	sprintf((char*)context, "%s", values[0]);
	return SQLITE_OK;
}

int cbfunc_GetAVIFilename(void *context, int count, char **values, char ** columnName){
	sprintf((char*)context, "%s", values[0]);
	return SQLITE_OK;
}

int cbfunc_GetListTotal(void *context, int count, char **values, char ** columnName){
	strncpy((char*)context, values[0], strlen(values[0]));
	return SQLITE_OK;
}

int busy_handler(void *data, int retry){
	busy_handler_attr* attr = (busy_handler_attr*)data;

	if (retry < attr->max_retry) {
		fprintf(stderr, "Hits SQLITE_BUSY %d times, retry again.\n", retry);
		sqlite3_sleep(attr->sleep_ms);
		return 1;
	}
	fprintf(stderr, "Error: retried %d times, exit.\n", retry);
	return 0;
}

int setLogMsg(char *_msg){
	char sql_cmd_buffer[SQL_CMD_BUFFER_LEN];
	char *error_report = NULL, timeformat[24];
	int ret = -1, total = getNumOfDBTotalRecord(ENUM_DB_TYPE_IS_LOG);
	time_t tCurrentTime;
	struct tm* tPtr;
	sqlite3 *db;
	busy_handler_attr bh_attr;

	if(strlen(_msg) >= MAX_LEN_OF_LOG_MSG){
		fprintf(stderr, "[%s][%s][%d] Msg too long %d\n", __FILE__, __func__, __LINE__, strlen(_msg));
		log_message("[%s][%s][%d] Msg too long %d\n", __FILE__, __func__, __LINE__, strlen(_msg));
		return -1;
	}

	// fprintf(stderr, "[%s][%d] (total:%d)%s\n", __func__, __LINE__, total, _msg);
	// log_message("[%s][%d] (total:%d)%s", __func__, __LINE__, total, _msg);

	if((ret = sqlite3_open(LOG_DB_FILE, &db)) != SQLITE_OK ){
		findErrMsg(__func__, __LINE__, ret, EMPTY_STR, EMPTY_STR);
		sqlite3_close(db);
		return -1;
	}

	bh_attr.max_retry = 100;
	bh_attr.sleep_ms = 100;
	sqlite3_busy_handler(db, busy_handler, &bh_attr);

	sqlite3_mutex_enter(sqlite3_db_mutex(db));
	if((ret = sqlite3_exec(db , SQL_CMD_CREATE_TABLE_LOG , NULL , NULL , &error_report)) != SQLITE_OK){
		findErrMsg(__func__, __LINE__, ret, error_report, SQL_CMD_CREATE_TABLE_LOG);
		sqlite3_close(db);
		return -1;
	}

	time(&tCurrentTime);
	tPtr = localtime(&tCurrentTime);
	memset(timeformat, '\0', sizeof(timeformat));

	sprintf(timeformat, "%04d-%02d-%02d %02d:%02d:%02d", tPtr->tm_year+1900, tPtr->tm_mon+1, tPtr->tm_mday, tPtr->tm_hour, tPtr->tm_min, tPtr->tm_sec);

	if(total >= MAX_LOG_RECORD){
		make_sqlite_cmd(sql_cmd_buffer, "DELETE FROM %s WHERE id > %d;", LOG_TABLE_NAME, MAX_LOG_RECORD);
		if((ret = sqlite3_exec(db, sql_cmd_buffer, NULL, NULL, &error_report)) != SQLITE_OK){
			findErrMsg(__func__, __LINE__, ret, error_report, sql_cmd_buffer);
			sqlite3_close(db);
			return -1;
		}
		make_sqlite_cmd(sql_cmd_buffer, "DELETE FROM %s WHERE id <= 0;", LOG_TABLE_NAME);
		if((ret = sqlite3_exec(db, sql_cmd_buffer, NULL, NULL, &error_report)) != SQLITE_OK){
			findErrMsg(__func__, __LINE__, ret, error_report, sql_cmd_buffer);
			sqlite3_close(db);
			return -1;
		}
		make_sqlite_cmd(sql_cmd_buffer, "DELETE FROM %s WHERE id = 1;", LOG_TABLE_NAME);
		if((ret = sqlite3_exec(db, sql_cmd_buffer, NULL, NULL, &error_report)) != SQLITE_OK){
			findErrMsg(__func__, __LINE__, ret, error_report, sql_cmd_buffer);
			sqlite3_close(db);
			return -1;
		}
		make_sqlite_cmd(sql_cmd_buffer, "UPDATE %s set id = id-1;", LOG_TABLE_NAME);
		if((ret = sqlite3_exec(db, sql_cmd_buffer, NULL, NULL, &error_report)) != SQLITE_OK){
			findErrMsg(__func__, __LINE__, ret, error_report, sql_cmd_buffer);
			sqlite3_close(db);
			return -1;
		}
		make_sqlite_cmd(sql_cmd_buffer, "INSERT INTO %s VALUES(%d, '%s', '%s');", LOG_TABLE_NAME, MAX_LOG_RECORD, timeformat, _msg);
	}else{
		make_sqlite_cmd(sql_cmd_buffer, "INSERT INTO %s VALUES(NULL, '%s', '%s');", LOG_TABLE_NAME, timeformat, _msg);
	}

	if((ret = sqlite3_exec(db, sql_cmd_buffer, NULL, NULL, &error_report)) != SQLITE_OK){
		findErrMsg(__func__, __LINE__, ret, error_report, sql_cmd_buffer);
		sqlite3_close(db);
		return -1;
	}
	make_sqlite_cmd(sql_cmd_buffer, "vacuum;");
	if((ret = sqlite3_exec(db , sql_cmd_buffer , NULL, NULL, &error_report)) != SQLITE_OK ){
		findErrMsg(__func__, __LINE__, ret, error_report, sql_cmd_buffer);
		sqlite3_close(db);
		return -1;
	}
	sqlite3_mutex_leave(sqlite3_db_mutex(db));

	sqlite3_close(db);

	return 0;
}

int getLogMsg(int _offset, char *_msg_buf){
	char sql_cmd_buffer[SQL_CMD_BUFFER_LEN];
	char *error_report = NULL;
	int ret = -1;
	sqlite3 *db;
	busy_handler_attr bh_attr;
	memset(_msg_buf, '\0', MAX_TOTAL_LEN_OF_LOG_MSG);

	if((ret = sqlite3_open(LOG_DB_FILE, &db)) != SQLITE_OK ){
		findErrMsg(__func__, __LINE__, ret, EMPTY_STR, EMPTY_STR);
		sqlite3_close(db);
		return -1;
	}

	bh_attr.max_retry = 100;
	bh_attr.sleep_ms = 100;
	sqlite3_busy_handler(db, busy_handler, &bh_attr);

	sqlite3_mutex_enter(sqlite3_db_mutex(db));

	g_len_ret2 = 0;
	make_sqlite_cmd(sql_cmd_buffer, "SELECT datetime,msg FROM %s ORDER BY ID DESC LIMIT 20 OFFSET %d;", LOG_TABLE_NAME, _offset);
	if((ret = sqlite3_exec(db, sql_cmd_buffer, cbfunc_ShowLogList, (void*)_msg_buf, &error_report)) != SQLITE_OK ){
		findErrMsg(__func__, __LINE__, ret, error_report, sql_cmd_buffer);
		sqlite3_close(db);
		return -1;
	}
	_msg_buf[g_len_ret2-1] = '\0';
	sqlite3_mutex_leave(sqlite3_db_mutex(db));
	sqlite3_close(db);

	if(strlen(_msg_buf) <= 0){
		log_message("[%s][%d] log is empty", __func__, __LINE__);
		return -1;
	}/*else{
		log_message("%s\n", _msg_buf);
	}*/
	return 0;
}

int getLogMsgbyIndex(int _index, char *_msg_buf){
	char sql_cmd_buffer[SQL_CMD_BUFFER_LEN];
	char *error_report = NULL;
	int ret = -1;
	sqlite3 *db;
	busy_handler_attr bh_attr;
	memset(_msg_buf, '\0', MAX_LEN_OF_LOG_MSG);

	if((ret = sqlite3_open(LOG_DB_FILE, &db)) != SQLITE_OK ){
		findErrMsg(__func__, __LINE__, ret, EMPTY_STR, EMPTY_STR);
		sqlite3_close(db);
		return -1;
	}

	bh_attr.max_retry = 100;
	bh_attr.sleep_ms = 100;
	sqlite3_busy_handler(db, busy_handler, &bh_attr);

	sqlite3_mutex_enter(sqlite3_db_mutex(db));

	make_sqlite_cmd(sql_cmd_buffer, "SELECT datetime,msg FROM %s WHERE id = %d;", LOG_TABLE_NAME, _index);
	if((ret = sqlite3_exec(db, sql_cmd_buffer, cbfunc_ShowLogList, (void*)_msg_buf, &error_report)) != SQLITE_OK ){
		findErrMsg(__func__, __LINE__, ret, error_report, sql_cmd_buffer);
		sqlite3_close(db);
		return -1;
	}
	sqlite3_mutex_leave(sqlite3_db_mutex(db));
	sqlite3_close(db);

	if(strlen(_msg_buf) <= 0){
		log_message("[%s][%d] log(%d) is empty", __func__, __LINE__, _index);
		return -1;
	}
	return 0;
}

int delAllLog(void){
	char sql_cmd_buffer[SQL_CMD_BUFFER_LEN];
	char *error_report = NULL;
	int ret = -1;
	sqlite3 *db;
	busy_handler_attr bh_attr;

	if((ret = sqlite3_open(LOG_DB_FILE, &db)) != SQLITE_OK){
		findErrMsg(__func__, __LINE__, ret, EMPTY_STR, EMPTY_STR);
		sqlite3_close(db);
		return -1;
	}

	bh_attr.max_retry = 100;
	bh_attr.sleep_ms = 100;
	sqlite3_busy_handler(db, busy_handler, &bh_attr);

	sqlite3_mutex_enter(sqlite3_db_mutex(db));
	make_sqlite_cmd(sql_cmd_buffer, "DELETE FROM %s;", LOG_TABLE_NAME);
	if((ret = sqlite3_exec(db, sql_cmd_buffer, NULL, NULL, &error_report)) != SQLITE_OK){
		findErrMsg(__func__, __LINE__, ret, error_report, sql_cmd_buffer);
		sqlite3_close(db);
		return -1;
	}
	make_sqlite_cmd(sql_cmd_buffer, "vacuum;");
	if((ret = sqlite3_exec(db , sql_cmd_buffer , NULL, NULL, &error_report)) != SQLITE_OK ){
		findErrMsg(__func__, __LINE__, ret, error_report, sql_cmd_buffer);
		sqlite3_close(db);
		return -1;
	}
	sqlite3_mutex_leave(sqlite3_db_mutex(db));
	sqlite3_close(db);

	// //// if necessary
	// if(!access(LOG_DB_FILE, F_OK))  unlink(LOG_DB_FILE);

	return 0;
}

int getLogTotalNum(void){
	char *error_report = NULL;
	char total[10];
	int ret = -1;
	int retry = 0;
	busy_handler_attr bh_attr;
	sqlite3 *db;

	if(access(LOG_DB_FILE, F_OK) != 0){
		log_message("[%s][%d] %s not exist, First boot.", __func__, __LINE__, LOG_DB_FILE);
		return 0;
	}

	memset(total, '\0', sizeof(total));

	if((ret = sqlite3_open(LOG_DB_FILE, &db)) != SQLITE_OK){
		findErrMsg(__func__, __LINE__, ret, EMPTY_STR, EMPTY_STR);
		sqlite3_close(db);
		return -1;
	}

	bh_attr.max_retry = 100;
	bh_attr.sleep_ms = 100;
	sqlite3_busy_handler(db, busy_handler, &bh_attr);

	sqlite3_mutex_enter(sqlite3_db_mutex(db));
	if((ret = sqlite3_exec(db, SQL_CMD_TOTAL_CNT_QUERY, cbfunc_GetListTotal, (void*)total, &error_report)) != SQLITE_OK){
		findErrMsg(__func__, __LINE__, ret, error_report, SQL_CMD_TOTAL_CNT_QUERY);
		sqlite3_close(db);
		return -1;
	}
	sqlite3_mutex_leave(sqlite3_db_mutex(db));
	sqlite3_close(db);

	return atoi(total);
}

int getNumOfDBTotalRecord(int _db_type){
	char sql_cmd_buffer[SQL_CMD_BUFFER_LEN];
	char *error_report = NULL;
	char total[10], *db_file_path, *db_table_name;
	int ret = -1;
	sqlite3 *db;
	busy_handler_attr bh_attr;

	switch(_db_type){
		case ENUM_DB_TYPE_IS_LOG:
			db_file_path = LOG_DB_FILE;
			db_table_name = LOG_TABLE_NAME;
			break;
		case ENUM_DB_TYPE_IS_AVIREC:
			db_file_path = AVI_REC_DB_FILE;
			db_table_name = AVI_REC_TABLE_NAME;
			break;
		case ENUM_DB_TYPE_IS_AVIEVENT:
			db_file_path = AVI_EVENT_DB_FILE;
			db_table_name = AVI_EVENT_TABLE_NAME;
			break;
		default:
			fprintf(stderr, "[%s][%d] Unknown (%d)\n", __func__, __LINE__, _db_type);
			log_message("[%s][%d] Unknown (%d)\n", __func__, __LINE__, _db_type);
			return -1;
	};

	memset(total, '\0', sizeof(total));

	if((ret = sqlite3_open(db_file_path, &db)) != SQLITE_OK){
		findErrMsg(__func__, __LINE__, ret, EMPTY_STR, EMPTY_STR);
		sqlite3_close(db);
		return -1;
	}

	bh_attr.max_retry = 100;
	bh_attr.sleep_ms = 100;
	sqlite3_busy_handler(db, busy_handler, &bh_attr);

	sqlite3_mutex_enter(sqlite3_db_mutex(db));

	make_sqlite_cmd(sql_cmd_buffer, "SELECT COUNT(*) FROM %s;", db_table_name);
	if((ret = sqlite3_exec(db, sql_cmd_buffer, cbfunc_GetListTotal, (void*)total, &error_report)) != SQLITE_OK){
		findErrMsg(__func__, __LINE__, ret, error_report, sql_cmd_buffer);
		sqlite3_close(db);
		return -1;
	}
	sqlite3_mutex_leave(sqlite3_db_mutex(db));
	sqlite3_close(db);

	return atoi(total);
}

int insertAVIRecord2DB(int _rec_media, char *_filename){
	char sql_cmd_buffer[SQL_CMD_BUFFER_LEN];
    struct stat sb;
    struct tm* tPtr;
    int ret, len;
    char *error_report = NULL, *db_file_path, *db_creat_cmd, *db_table_name;
    char *tmpstr, avi_filename[64], avi_file_create_time[32], timeformat[24], tmp_filename[128];
    sqlite3 *db;
    busy_handler_attr bh_attr;

	switch(_rec_media){
		case ENUM_REC_MEDIA_IS_AVIREC:
			db_file_path = AVI_REC_DB_FILE;
			db_creat_cmd = CMD_CREATE_TABLE_AVIREC;
			db_table_name = AVI_REC_TABLE_NAME;
			break;
		case ENUM_REC_MEDIA_IS_AVIEVENT:
			db_file_path = AVI_EVENT_DB_FILE;
			db_creat_cmd = SQL_CMD_CREATE_TABLE_AVIEVENT;
			db_table_name = AVI_EVENT_TABLE_NAME;
			break;
		default:
			fprintf(stderr, "[%s][%d] Unknown (%d)\n", __func__, __LINE__, _rec_media);
			log_message("[%s][%d] Unknown (%d)\n", __func__, __LINE__, _rec_media);
			return -1;
	};

	sprintf(tmp_filename, "%s", _filename);
	if (stat(tmp_filename, &sb) == -1) {
		fprintf(stderr, "[%s][%d] stat() failed\n", __func__, __LINE__);
		log_message("[%s][%d] stat() failed\n", __func__, __LINE__);
		return -1;
	}

	tmpstr = strtok(tmp_filename, "/");
	while(tmpstr != NULL){
		memset(avi_filename, '\0', sizeof(avi_filename));
		strncpy(avi_filename, tmpstr, strlen(tmpstr));
		tmpstr = strtok(NULL, "/");
	}

	if((ret = sqlite3_open(db_file_path, &db)) != SQLITE_OK ){
		findErrMsg(__func__, __LINE__, ret, EMPTY_STR, EMPTY_STR);
		sqlite3_close(db);
		return -1;
	}

	bh_attr.max_retry = 100;
	bh_attr.sleep_ms = 100;
	sqlite3_busy_handler(db, busy_handler, &bh_attr);

    sqlite3_mutex_enter(sqlite3_db_mutex(db));
    if((ret = sqlite3_exec(db , db_creat_cmd , NULL , NULL , &error_report)) != SQLITE_OK){
        findErrMsg(__func__, __LINE__, ret, error_report, db_creat_cmd);
        sqlite3_close(db);
        return -1;
    }

    len = sprintf(avi_file_create_time, "%s", ctime(&sb.st_mtime));
    avi_file_create_time[len-1] = '\0';
    tPtr = localtime(&sb.st_mtime);
    memset(timeformat, '\0', sizeof(timeformat));

    sprintf(timeformat, "%04d-%02d-%02d %02d:%02d:%02d", tPtr->tm_year+1900, tPtr->tm_mon+1, tPtr->tm_mday, tPtr->tm_hour, tPtr->tm_min, tPtr->tm_sec);

    make_sqlite_cmd(sql_cmd_buffer, "INSERT INTO %s VALUES(NULL, '%s', %ld, '%s', '%s');", db_table_name, avi_filename, sb.st_size, timeformat, avi_file_create_time);
    if((ret = sqlite3_exec(db, sql_cmd_buffer, NULL, NULL, &error_report)) != SQLITE_OK){
        findErrMsg(__func__, __LINE__, ret, error_report, sql_cmd_buffer);
        sqlite3_close(db);
        return -1;
    }
    make_sqlite_cmd(sql_cmd_buffer, "vacuum;");
    if((ret = sqlite3_exec(db , sql_cmd_buffer , NULL, NULL, &error_report)) != SQLITE_OK ){
        findErrMsg(__func__, __LINE__, ret, error_report, sql_cmd_buffer);
        sqlite3_close(db);
        return -1;
    }

    sqlite3_mutex_leave(sqlite3_db_mutex(db));

    sqlite3_close(db);

    return 0;
}

int deleteAVIRecord2DB(int _rec_media){
	char sql_cmd_buffer[SQL_CMD_BUFFER_LEN];
	struct stat sb;
	struct tm* tPtr;
	int ret, len, o = 0;
	char *error_report = NULL, *db_file_path, *db_table_name;
	char *tmpstr, avi_filename[64], avi_file_full_path[128];
	sqlite3 *db;
	busy_handler_attr bh_attr;

	memset(avi_filename, '\0', sizeof(avi_filename));
	memset(avi_file_full_path, '\0', sizeof(avi_file_full_path));

	switch(_rec_media){
		case ENUM_REC_MEDIA_IS_AVIREC:
			db_file_path = AVI_REC_DB_FILE;
			db_table_name = AVI_REC_TABLE_NAME;
			o += sprintf(&avi_file_full_path[o], "%s/Backup/", SDCARD_MEDIA_PATH);
			break;
		case ENUM_REC_MEDIA_IS_AVIEVENT:
			db_file_path = AVI_EVENT_DB_FILE;
			db_table_name = AVI_EVENT_TABLE_NAME;
			o += sprintf(&avi_file_full_path[o], "%s/Event/", SDCARD_MEDIA_PATH);
			break;
		default:
			fprintf(stderr, "[%s][%d] Unknown (%d)\n", __func__, __LINE__, _rec_media);
			log_message("[%s][%d] Unknown (%d)\n", __func__, __LINE__, _rec_media);
			return -1;
	};

	if((ret = sqlite3_open(db_file_path, &db)) != SQLITE_OK ){
		findErrMsg(__func__, __LINE__, ret, EMPTY_STR, EMPTY_STR);
		sqlite3_close(db);
		return -1;
	}

	bh_attr.max_retry = 100;
	bh_attr.sleep_ms = 100;
	sqlite3_busy_handler(db, busy_handler, &bh_attr);

	sqlite3_mutex_enter(sqlite3_db_mutex(db));
	make_sqlite_cmd(sql_cmd_buffer, "SELECT filename FROM %s WHERE id = 1;", db_table_name);
	if((ret = sqlite3_exec(db, sql_cmd_buffer, cbfunc_GetAVIFilename, (void*)avi_filename, &error_report)) != SQLITE_OK){
		findErrMsg(__func__, __LINE__, ret, error_report, sql_cmd_buffer);
		sqlite3_close(db);
		return -1;
	}
	o += sprintf(&avi_file_full_path[o], "%s", avi_filename);
	make_sqlite_cmd(sql_cmd_buffer, "DELETE FROM %s WHERE id = 1;", db_table_name);
	if((ret = sqlite3_exec(db, sql_cmd_buffer, NULL, NULL, &error_report)) != SQLITE_OK){
		findErrMsg(__func__, __LINE__, ret, error_report, sql_cmd_buffer);
		sqlite3_close(db);
		return -1;
	}
	make_sqlite_cmd(sql_cmd_buffer, "UPDATE %s set id=id-1;", db_table_name);
	if((ret = sqlite3_exec(db, sql_cmd_buffer, NULL, NULL, &error_report)) != SQLITE_OK){
		findErrMsg(__func__, __LINE__, ret, error_report, sql_cmd_buffer);
		sqlite3_close(db);
		return -1;
	}
	make_sqlite_cmd(sql_cmd_buffer, "vacuum;");
	if((ret = sqlite3_exec(db , sql_cmd_buffer , NULL, NULL, &error_report)) != SQLITE_OK ){
		findErrMsg(__func__, __LINE__, ret, error_report, sql_cmd_buffer);
		sqlite3_close(db);
		return -1;
	}

	sqlite3_mutex_leave(sqlite3_db_mutex(db));

	sqlite3_close(db);
	if(!access(avi_file_full_path, F_OK)){
		unlink(avi_file_full_path);
		fprintf(stderr, "[%s][%d] %s was removed\n", __func__, __LINE__, avi_file_full_path);
		log_message("[%s][%d] %s was removed\n", __func__, __LINE__, avi_file_full_path);
	}

	return 0;
}

int queryAllAVIDB(int _rec_media, int _page, int _max_record_num_per_page, char *_db_buff){
	char sql_cmd_buffer[SQL_CMD_BUFFER_LEN];
	char *error_report = NULL, *db_file_path, *db_table_name;
	int ret = 0;
	sqlite3 *db;
	busy_handler_attr bh_attr;

	switch(_rec_media){
		case ENUM_REC_MEDIA_IS_AVIREC:
			db_file_path = AVI_REC_DB_FILE;
			db_table_name = AVI_REC_TABLE_NAME;
			break;
		case ENUM_REC_MEDIA_IS_AVIEVENT:
			db_file_path = AVI_EVENT_DB_FILE;
			db_table_name = AVI_EVENT_TABLE_NAME;
			break;
		default:
			fprintf(stderr, "[%s][%d] Unknown (%d)\n", __func__, __LINE__, _rec_media);
			log_message("[%s][%d] Unknown (%d)\n", __func__, __LINE__, _rec_media);
			return -1;
	};

	if((ret = sqlite3_open(db_file_path, &db)) != SQLITE_OK ){
		findErrMsg(__func__, __LINE__, ret, EMPTY_STR, EMPTY_STR);
		sqlite3_close(db);
		return -1;
	}

	bh_attr.max_retry = 100;
	bh_attr.sleep_ms = 100;
	sqlite3_busy_handler(db, busy_handler, &bh_attr);

	sqlite3_mutex_enter(sqlite3_db_mutex(db));
	make_sqlite_cmd(sql_cmd_buffer, "SELECT filename, datetime, filesize/1024 FROM %s ORDER BY datetime DESC LIMIT %d OFFSET %d;", db_table_name, _max_record_num_per_page, ((_page-1)*_max_record_num_per_page));
	g_len_ret1 = 0;
	if((ret = sqlite3_exec(db, sql_cmd_buffer, cbfunc_ShowAVIList, (void*)_db_buff, &error_report)) != SQLITE_OK){
		findErrMsg(__func__, __LINE__, ret, error_report, sql_cmd_buffer);
		sqlite3_close(db);
		return -1;
	}
	_db_buff[g_len_ret1-1] = '\0';
	sqlite3_mutex_leave(sqlite3_db_mutex(db));

	sqlite3_close(db);

	return 0;
}

int queryAllAVIDBByDateTime(int _rec_media, int _page, int _maxNumOfPage, char *_db_buff, struct tm *_start_tm, struct tm *_end_tm){
	char sql_cmd_buffer[SQL_CMD_BUFFER_LEN];
	char *error_report = NULL, *db_file_path, *db_table_name, startTimeFMT[20], endTimeFMT[20];
	int ret = 0;
	sqlite3 *db;
	busy_handler_attr bh_attr;

	switch(_rec_media){
		case ENUM_REC_MEDIA_IS_AVIREC:
			db_file_path = AVI_REC_DB_FILE;
			db_table_name = AVI_REC_TABLE_NAME;
			break;
		case ENUM_REC_MEDIA_IS_AVIEVENT:
			db_file_path = AVI_EVENT_DB_FILE;
			db_table_name = AVI_EVENT_TABLE_NAME;
			break;
		default:
			fprintf(stderr, "[%s][%d] Unknown (%d)\n", __func__, __LINE__, _rec_media);
			log_message("[%s][%d] Unknown (%d)\n", __func__, __LINE__, _rec_media);
			return -1;
	};

	memset(startTimeFMT, '\0', sizeof(startTimeFMT));
	memset(endTimeFMT, '\0', sizeof(endTimeFMT));

	sprintf(startTimeFMT, "%04d-%02d-%02d %02d:%02d:%02d", _start_tm->tm_year+1900, _start_tm->tm_mon+1, _start_tm->tm_mday, _start_tm->tm_hour, _start_tm->tm_min, _start_tm->tm_sec);
	sprintf(endTimeFMT, "%04d-%02d-%02d %02d:%02d:%02d", _end_tm->tm_year+1900, _end_tm->tm_mon+1, _end_tm->tm_mday, _end_tm->tm_hour, _end_tm->tm_min, _end_tm->tm_sec);

	if((ret = sqlite3_open(db_file_path, &db)) != SQLITE_OK ){
		findErrMsg(__func__, __LINE__, ret, EMPTY_STR, EMPTY_STR);
		sqlite3_close(db);
		return -1;
	}

	bh_attr.max_retry = 100;
	bh_attr.sleep_ms = 100;
	sqlite3_busy_handler(db, busy_handler, &bh_attr);

    sqlite3_mutex_enter(sqlite3_db_mutex(db));
	make_sqlite_cmd(sql_cmd_buffer, "SELECT filename, datetime, filesize/1024 FROM %s WHERE strftime('%%Y-%%m-%%d %%H:%%M:%%S', createTime) > '%s' AND strftime('%%Y-%%m-%%d %%H:%%M:%%S', createTime) < '%s' ORDER BY datetime DESC LIMIT %d OFFSET %d;", db_table_name, startTimeFMT, endTimeFMT, _maxNumOfPage, ((_page-1)*_maxNumOfPage));
	g_len_ret1 = 0;
	if((ret = sqlite3_exec(db, sql_cmd_buffer, cbfunc_ShowAVIList, (void*)_db_buff, &error_report)) != SQLITE_OK){
		findErrMsg(__func__, __LINE__, ret, error_report, sql_cmd_buffer);
		sqlite3_close(db);
		return -1;
	}
    _db_buff[g_len_ret1-1] = '\0';
    sqlite3_mutex_leave(sqlite3_db_mutex(db));

    sqlite3_close(db);

    return 0;
}

int delAVIFileFromDB(int _rec_media, char *_file_dir, char *_filename){
	//// for sdcard.cgi delete function
	char sql_cmd_buffer[SQL_CMD_BUFFER_LEN];
	char *error_report = NULL, *db_file_path, *db_table_name, avi_file_path[128], avi_id[10];
	int ret = 0;
	sqlite3 *db;
	busy_handler_attr bh_attr;

	switch(_rec_media){
		case ENUM_REC_MEDIA_IS_AVIREC:
			db_file_path = AVI_REC_DB_FILE;
			db_table_name = AVI_REC_TABLE_NAME;
			break;
		case ENUM_REC_MEDIA_IS_AVIEVENT:
			db_file_path = AVI_EVENT_DB_FILE;
			db_table_name = AVI_EVENT_TABLE_NAME;
			break;
		default:
			fprintf(stderr, "[%s][%d] Unknown (%d)\n", __func__, __LINE__, _rec_media);
			log_message("[%s][%d] Unknown (%d)\n", __func__, __LINE__, _rec_media);
			return -1;
	};

    memset(avi_id, '\0', sizeof(avi_id));

    if((ret = sqlite3_open(db_file_path, &db)) != SQLITE_OK){
        findErrMsg(__func__, __LINE__, ret, EMPTY_STR, EMPTY_STR);
        sqlite3_close(db);
        return -1;
    }

	bh_attr.max_retry = 100;
	bh_attr.sleep_ms = 100;
	sqlite3_busy_handler(db, busy_handler, &bh_attr);

    sqlite3_mutex_enter(sqlite3_db_mutex(db));
    make_sqlite_cmd(sql_cmd_buffer, "SELECT id FROM %s WHERE filename = '%s';", db_table_name, _filename);
    if((ret = sqlite3_exec(db, sql_cmd_buffer, cbfunc_GetAVIID, (void*)avi_id, &error_report)) != SQLITE_OK){
        findErrMsg(__func__, __LINE__, ret, error_report, sql_cmd_buffer);
        sqlite3_close(db);
        return -1;
    }
    make_sqlite_cmd(sql_cmd_buffer, "DELETE FROM %s WHERE filename = '%s';", db_table_name, _filename);
    if((ret = sqlite3_exec(db, sql_cmd_buffer, NULL, NULL, &error_report)) != SQLITE_OK){
        findErrMsg(__func__, __LINE__, ret, error_report, sql_cmd_buffer);
        sqlite3_close(db);
        return -1;
    }
    make_sqlite_cmd(sql_cmd_buffer, "UPDATE %s set id=id-1 WHERE id > %s;", db_table_name, avi_id);
    if((ret = sqlite3_exec(db, sql_cmd_buffer, NULL, NULL, &error_report)) != SQLITE_OK){
        findErrMsg(__func__, __LINE__, ret, error_report, sql_cmd_buffer);
        sqlite3_close(db);
        return -1;
    }
    make_sqlite_cmd(sql_cmd_buffer, "vacuum;");
    if((ret = sqlite3_exec(db , sql_cmd_buffer , NULL, NULL, &error_report)) != SQLITE_OK ){
        findErrMsg(__func__, __LINE__, ret, error_report, sql_cmd_buffer);
        sqlite3_close(db);
        return -1;
    }
    sqlite3_mutex_leave(sqlite3_db_mutex(db));
    sqlite3_close(db);

    memset(avi_file_path, '\0', sizeof(avi_file_path));
    sprintf(avi_file_path, "%s/%s", _file_dir, _filename);
    if(!access(avi_file_path, F_OK)){
        unlink(avi_file_path);
        log_message("[%s][%d] %s was removed !!!", __func__, __LINE__, avi_file_path);
    }

    return 0;
}



/***************************************************************************** 
** 
** Copyright (c) 2012~2112 ShangHai Infotm Ltd all rights reserved. 
** 
** Use of Infotm's code is governed by terms and conditions 
** stated in the accompanying licensing statement. 
** 
**      
** Revision History: 
** ----------------- 
** v1.1.0	leo@2012/03/17: first commit.
** v1.2.1	leo@2012/04/05: 
**				1. statc not include itself.
**				2. In mm_test, add read/write per byte test to allocated buffer.
** v1.2.2	leo@2012/04/20: add MAP_USER_ADDR_TEST, because now don't support map-user-addr
**				test, so undefined it.
** v1.2.4	leo@2012/04/28: tested MAP_USER_ADDR_TEST ok.
**			leo@2012/05/12: add TB_ARGP_KEY_MM_MAP_ALLOC_MODE to test map user viraddr allocated 
**			by malloc() and alc_alloc() seperately.
**			
*****************************************************************************/ 

#include <InfotmMedia.h>
#include <IM_buffallocapi.h>

#define DBGINFO		0
#define DBGWARN		1
#define DBGERR		1
#define DBGTIP		1

#define INFOHEAD	"ALCTST_I"
#define WARNHEAD	"ALCTST_I"
#define ERRHEAD		"ALCTST_I"
#define TIPHEAD		"ALCTST_I"


#define PRINTMSG(str)	printf str; printf("\r\n")

#define MAP_USER_ADDR_TEST

#define TB_ARGP_KEY_FUNCTION		0x1000
#define TB_ARGP_KEY_STATC_OWNER		0x2001
#define TB_ARGP_KEY_STATC_PERIODS	0x2002
#define TB_ARGP_KEY_MM_MINSIZE		0x3000
#define TB_ARGP_KEY_MM_MAXSIZE		0x3001
#define TB_ARGP_KEY_MM_NUMBER		0x3002
#define TB_ARGP_KEY_MM_DELAY		0x3003
#define TB_ARGP_KEY_MM_RANDOM_FREE	0x3004
#define TB_ARGP_KEY_MM_ALLOC_MODE	0x3005
#define TB_ARGP_KEY_MM_MAP_ALLOC_MODE	0x3006

#define TB_ARGP_KEY_FUNCTION_DEFAULT		"unknown"
#define TB_ARGP_KEY_STATC_OWNER_DEFAULT		"unknown"
#define TB_ARGP_KEY_STATC_OWNER_PERIODS		"0"
#define TB_ARGP_KEY_MM_MINSIZE_DEFAULT		"1024"
#define TB_ARGP_KEY_MM_MAXSIZE_DEFAULT		"1*1024*1024"
#define TB_ARGP_KEY_MM_NUMBER_DEFAULT		"10"
#define TB_ARGP_KEY_MM_DELAY_DEFAULT		"100"
#define TB_ARGP_KEY_MM_RANDOM_FREE_DEFAULT	"1"
#define TB_ARGP_KEY_MM_ALLOC_MODE_DEFAULT	"0"
#define TB_ARGP_KEY_MM_MAP_ALLOC_MODE_DEFAULT	"0"



static argp_key_table_entry_t gArgKeyTable[] = 
{
	{TB_ARGP_KEY_FUNCTION, (IM_TCHAR)'f', IM_STR("func"), IM_STR("[statc]--statistic function,  [mm]--alloc/free function. default [unknown].")},
	{TB_ARGP_KEY_STATC_OWNER, (IM_TCHAR)'o', IM_STR("owner"), IM_STR("the owner. default [unknown].")},
	{TB_ARGP_KEY_STATC_PERIODS, (IM_TCHAR)'p', IM_STR("periods"), IM_STR("statc periods, 0 once, unit ms. default [0].")},
	{TB_ARGP_KEY_MM_ALLOC_MODE, 0, IM_STR("mode"), IM_STR("alloc mode, 0--virtual, 1--phy_linear_must, 2--phy_linear_prefer, 3--map, default [0].")},
	{TB_ARGP_KEY_MM_MAP_ALLOC_MODE, 0, IM_STR("mapalcmode"), IM_STR("alloc memory mode in map user addr test, 0--malloc, 1--phy_linear_must, 2--phy_linear_prefer. default [0].")},
	{TB_ARGP_KEY_MM_MINSIZE, 0, IM_STR("min"), IM_STR("min size. default [1024].")},
	{TB_ARGP_KEY_MM_MAXSIZE, 0, IM_STR("max"), IM_STR("max size. default [1*1024*1024].")},
	{TB_ARGP_KEY_MM_NUMBER, 0, IM_STR("num"), IM_STR("number. default [10].")},
	{TB_ARGP_KEY_MM_DELAY, 0, IM_STR("delay"), IM_STR("delay between per alloc and per free, ms unit. default [100].")},
	{TB_ARGP_KEY_MM_RANDOM_FREE, 0, IM_STR("random"), IM_STR("free in random order, 0--ordinal, 1--random. default [1].")},
};


typedef struct{
	IM_TCHAR	*owner;
	IM_INT32	periods;	// ms unit.
}alc_tb_statc_config_t;

#define MAP_ALC_MODE_MALLOC			0
#define MAP_ALC_MODE_PHY_LINEAR		1
#define MAP_ALC_MODE_PHY_NONLINEAR	2

typedef struct{
	IM_INT32	allocMode;
	IM_INT32	minSize;
	IM_INT32	maxSize;
	IM_INT32	number;
	IM_INT32	delay;
	IM_BOOL		randomFree;
	IM_INT32	mapAlcMode;	// MAP_ALC_MODE_xxx.
}alc_tb_mm_config_t;

typedef struct{
	IM_INT32		func;	// 0--statc, 1--mm
	alc_tb_statc_config_t	statcConfig;
	alc_tb_mm_config_t	mmConfig;
}alc_tb_config_t;

static alc_tb_config_t gTbConfig;


static void menu_help()
{
	IM_TCHAR optstr[256] = {0};
	IM_INT32 i;

	PRINTMSG((IM_STR("#############################################################")));
	for(i=0; i<sizeof(gArgKeyTable) / sizeof(gArgKeyTable[0]); i++){
		memset((void *)optstr, 0, sizeof(optstr));
		if(gArgKeyTable[i].shortOpt != 0){
			sprintf(optstr, "-%c", gArgKeyTable[i].shortOpt);// migrate has problem???
			//PRINTMSG((IM_STR("-%c "), gArgKeyTable[i].shortOpt));
		}
		if(gArgKeyTable[i].longOpt != IM_NULL){
			sprintf(optstr, "%s\t--%s", optstr, gArgKeyTable[i].longOpt);// migrate has problem???
			//PRINTMSG((IM_STR("--%s"), gArgKeyTable[i].longOpt));
		}
		PRINTMSG((IM_STR("%s: %s"), optstr, gArgKeyTable[i].desc));
	}
	PRINTMSG((IM_STR("#############################################################")));
}

static argp_key_table_entry_t * get_keytable_entry(IM_INT32 key)
{
	IM_INT32 i;
	for(i=0; i<sizeof(gArgKeyTable) / sizeof(gArgKeyTable[0]); i++){
		if(key == gArgKeyTable[i].key){
			return &gArgKeyTable[i];
		}
	}

	return IM_NULL;
}

static void tb_config_init(IM_INT32 argc, IM_TCHAR *argv[])
{
	IM_TCHAR *funcString = IM_NULL;

	// init default config.
	gTbConfig.func = -1;

	gTbConfig.statcConfig.owner = IM_NULL;
	gTbConfig.statcConfig.periods = 0;

	gTbConfig.mmConfig.allocMode = 0;
	gTbConfig.mmConfig.minSize = 1024;
	gTbConfig.mmConfig.maxSize = 1 * 1024 * 1024;
	gTbConfig.mmConfig.number = 10;
	gTbConfig.mmConfig.delay = 100;
	gTbConfig.mmConfig.randomFree = IM_TRUE;
	gTbConfig.mmConfig.mapAlcMode = MAP_ALC_MODE_MALLOC;

	//
	if(argp_get_key_string(get_keytable_entry(TB_ARGP_KEY_FUNCTION), &funcString, argc, argv) == IM_RET_OK){
		if(im_oswl_strcmp(funcString, IM_STR("statc")) == 0){
			gTbConfig.func = 0;	// statc
		}else if(im_oswl_strcmp(funcString, IM_STR("mm")) == 0){
			gTbConfig.func = 1;	// mm
		}
	}
	if(gTbConfig.func == -1){
		menu_help();
		return;
	}

	argp_get_key_string(get_keytable_entry(TB_ARGP_KEY_STATC_OWNER), &gTbConfig.statcConfig.owner, argc, argv);
	argp_get_key_int(get_keytable_entry(TB_ARGP_KEY_STATC_PERIODS), (IM_INT32 *)&gTbConfig.statcConfig.periods, argc, argv);
	
	argp_get_key_int(get_keytable_entry(TB_ARGP_KEY_MM_ALLOC_MODE), (IM_INT32 *)&gTbConfig.mmConfig.allocMode, argc, argv);
	argp_get_key_int(get_keytable_entry(TB_ARGP_KEY_MM_MINSIZE), &gTbConfig.mmConfig.minSize, argc, argv);
	argp_get_key_int(get_keytable_entry(TB_ARGP_KEY_MM_MAXSIZE), &gTbConfig.mmConfig.maxSize, argc, argv);
	argp_get_key_int(get_keytable_entry(TB_ARGP_KEY_MM_NUMBER), &gTbConfig.mmConfig.number, argc, argv);
	argp_get_key_int(get_keytable_entry(TB_ARGP_KEY_MM_DELAY), &gTbConfig.mmConfig.delay, argc, argv);
	argp_get_key_int(get_keytable_entry(TB_ARGP_KEY_MM_RANDOM_FREE), (IM_INT32 *)&gTbConfig.mmConfig.randomFree, argc, argv);
	argp_get_key_int(get_keytable_entry(TB_ARGP_KEY_MM_MAP_ALLOC_MODE), (IM_INT32 *)&gTbConfig.mmConfig.mapAlcMode, argc, argv);

	//
#ifndef MAP_USER_ADDR_TEST
	if(gTbConfig.mmConfig.allocMode == 3){
		gTbConfig.mmConfig.allocMode = 0;
	}
#endif

	//
	IM_INFOMSG((IM_STR("func=%d"), gTbConfig.func));
	if(gTbConfig.statcConfig.owner != IM_NULL){
		IM_INFOMSG((IM_STR("statcConfig.owner=%s"), gTbConfig.statcConfig.owner));
	}
	IM_INFOMSG((IM_STR("statcConfig.periods=%d"), gTbConfig.statcConfig.periods));
	IM_INFOMSG((IM_STR("mmConfig.allocMode=%d"), gTbConfig.mmConfig.allocMode));
	IM_INFOMSG((IM_STR("mmConfig.minSize=%d"), gTbConfig.mmConfig.minSize));
	IM_INFOMSG((IM_STR("mmConfig.maxSize=%d"), gTbConfig.mmConfig.maxSize));
	IM_INFOMSG((IM_STR("mmConfig.number=%d"), gTbConfig.mmConfig.number));
	IM_INFOMSG((IM_STR("mmConfig.delay=%d"), gTbConfig.mmConfig.delay));
	IM_INFOMSG((IM_STR("mmConfig.randomFree=%d"), gTbConfig.mmConfig.randomFree));
	IM_INFOMSG((IM_STR("mmConfig.mapAlcMode=%d"), gTbConfig.mmConfig.mapAlcMode));
}

IM_INT32 get_random_between(IM_INT32 min, IM_INT32 max)
{
	IM_INT32 val;
	do{
		val = rand();
		if((val < min) || (val > max)){
			val %= (max - min + 1);
			val += min;
		}
	}while((val < min) || (val > max));
	return val;
}

IM_BOOL get_random_bool()
{
	IM_INT32 val = get_random_between(0, 0x1000);
	return (val & 0x01)?IM_TRUE:IM_FALSE;
}

static IM_RET statc_list_owner_buffer(ALCCTX alc, IM_TCHAR *owner, im_list_handle_t abfList)
{
	alc_buffer_t *abf;
	IM_INT32 totalSize = 0;

	im_list_clear(abfList);

	if(alc_statc_list_owner_buffer(alc, owner, abfList) != IM_RET_OK){
		IM_ERRMSG((IM_STR("alc_statc_list_owner_buffer() failed")));
		return IM_RET_FAILED;
	}

	PRINTMSG((IM_STR("===============owner:%s============="), owner));
	abf = (alc_buffer_t *)im_list_begin(abfList);
	while(abf != IM_NULL){
		PRINTMSG((IM_STR("vir_addr=0x%x, phy_addr=0x%x, size=%d, flag=0x%x, devAddr=0x%x, attri=0x%x"), 
		(IM_INT32)abf->buffer.vir_addr, abf->buffer.phy_addr, abf->buffer.size, abf->buffer.flag, abf->devAddr, abf->attri));

		totalSize += abf->buffer.size;
		abf = (alc_buffer_t *)im_list_erase(abfList, abf);
	}
	PRINTMSG((IM_STR("===============owner:%s totalUsedSize=%dKB============="), owner, totalSize >> 10));
	
	return IM_RET_OK;
}

static IM_RET statc_test(ALCCTX alc, IM_TCHAR *itself)
{
	im_mempool_handle_t mpl = IM_NULL;
	im_list_handle_t ownerList = IM_NULL;	// list of owner.
	im_list_handle_t abfList = IM_NULL;	// list of alcBuffer.
	IM_TCHAR *owner;

	mpl = im_mpool_init((func_mempool_malloc_t)malloc, (func_mempool_free_t)free);
	if(mpl == IM_NULL){
		return IM_RET_FAILED;
	}
	
	ownerList = im_list_init(sizeof(IM_TCHAR) * ALC_OWNER_LEN_MAX, mpl);
	if(ownerList == IM_NULL){
		goto Fail;
	}
	
	abfList = im_list_init(sizeof(alc_buffer_t), mpl);
	if(abfList == IM_NULL){
		goto Fail;
	}

	do{
		if(gTbConfig.statcConfig.owner != IM_NULL){
			owner = gTbConfig.statcConfig.owner;
			IM_JIF(statc_list_owner_buffer(alc, owner, abfList));	
		}else{
			im_list_clear(ownerList);
			if(alc_statc_list_owner(alc, ownerList) != IM_RET_OK){
				IM_ERRMSG((IM_STR("alc_statc_list_owner() failed")));
				goto Fail;
			}

			owner = (IM_TCHAR *)im_list_begin(ownerList);
			while(owner != IM_NULL){
				if(im_oswl_strcmp(owner, itself) != 0){
					IM_JIF(statc_list_owner_buffer(alc, owner, abfList));
				}
				owner = (IM_TCHAR *)im_list_erase(ownerList, owner);
			}
		}

		im_oswl_sleep(gTbConfig.statcConfig.periods);
	}while(gTbConfig.statcConfig.periods != 0);

	im_list_deinit(abfList);
	im_list_deinit(ownerList);
	im_mpool_deinit(mpl);
	return IM_RET_OK;
Fail:
	if(abfList){
		im_list_deinit(abfList);
	}
	if(ownerList){
		im_list_deinit(ownerList);
	}
	return IM_RET_FAILED;
}

static IM_RET mm_test(ALCCTX alc)
{
	IM_INT32 i, j, flag;
	im_mempool_handle_t mpl = IM_NULL;
	im_list_handle_t list = IM_NULL;	// list of IM_Buffer.
	IM_INT32 number = 0, size, length, tmp;
	IM_Buffer buffer, *pBuffer, mapBuffer;
	IM_UINT32 devAddr;

	mpl = im_mpool_init((func_mempool_malloc_t)malloc, (func_mempool_free_t)free);
	if(mpl == IM_NULL){
		return IM_RET_FAILED;
	}
	
	list = im_list_init(sizeof(IM_Buffer), mpl);
	if(list == IM_NULL){
		goto Fail;
	}

	do{
		if(get_random_bool()){
			size = get_random_between(gTbConfig.mmConfig.minSize, gTbConfig.mmConfig.maxSize);
			if(gTbConfig.mmConfig.allocMode == 0){
				flag = 0;
			}else if(gTbConfig.mmConfig.allocMode == 1){
				flag = ALC_FLAG_PHY_MUST;
			}else if(gTbConfig.mmConfig.allocMode == 2){
				flag = ALC_FLAG_PHY_LINEAR_PREFER;
			}else if(gTbConfig.mmConfig.allocMode == 3){
				flag = 0;	// map.
			}else{
				flag = 0;
			}

			if(flag != 0){
				if(IM_FAILED(alc_alloc(alc, size, &buffer, flag | ALC_FLAG_DEVADDR))){
					IM_ERRMSG((IM_STR("alc_alloc() failed")));
					goto Fail;
				}
				if(IM_FAILED(alc_get_devaddr(alc, &buffer, &devAddr))){
					IM_ERRMSG((IM_STR("alc_get_devaddr() failed")));
					goto Fail;
				}
				im_list_put_back(list, (void *)&buffer);
				
				PRINTMSG((IM_STR("alloc: vir_addr=0x%x, phy_addr=0x%x, size=%d, flag=0x%x, devAddr=0x%x"), 
					(IM_INT32)buffer.vir_addr, buffer.phy_addr, buffer.size, buffer.flag, devAddr));
				for(i=0; i<size; i+= 4096){
					for(j = 0; j<4096; j+=4){
						*(IM_INT32 *)((IM_INT32)buffer.vir_addr + i + j) = (IM_INT32)buffer.vir_addr + i + j;
						tmp = *(IM_INT32 *)((IM_INT32)buffer.vir_addr + i + j);
						if(tmp != (IM_INT32)buffer.vir_addr + i + j){
							IM_ERRMSG((IM_STR("read/write vir_addr=0x%x failed"), (IM_INT32)buffer.vir_addr+i+j));
							goto Fail;
						}
					}
				}
			}else{
				if(gTbConfig.mmConfig.allocMode == 3){
					if(gTbConfig.mmConfig.mapAlcMode == MAP_ALC_MODE_MALLOC){
						mapBuffer.vir_addr = malloc(size);
					}else if(gTbConfig.mmConfig.mapAlcMode == MAP_ALC_MODE_PHY_LINEAR){
						if(IM_FAILED(alc_alloc(alc, size, &mapBuffer, ALC_FLAG_PHY_MUST))){
							IM_ERRMSG((IM_STR("alc_alloc() failed")));
							goto Fail;
						}
					}else if(gTbConfig.mmConfig.mapAlcMode == MAP_ALC_MODE_PHY_NONLINEAR){
						if(IM_FAILED(alc_alloc(alc, size, &mapBuffer, ALC_FLAG_PHY_LINEAR_PREFER))){
							IM_ERRMSG((IM_STR("alc_alloc() failed")));
							goto Fail;
						}
					}else{
						mapBuffer.vir_addr = malloc(size);
					}

					if(IM_FAILED(alc_map_useraddr(alc, mapBuffer.vir_addr, size, &mapBuffer, &devAddr))){
						IM_ERRMSG((IM_STR("alc_map_useraddr() failed")));
						goto Fail;
					}
				}else{
					if(IM_FAILED(alc_alloc(alc, size, &mapBuffer, 0))){
						IM_ERRMSG((IM_STR("alc_alloc() failed")));
						goto Fail;
					}
				}
				im_list_put_back(list, (void *)&mapBuffer);
				
				PRINTMSG((IM_STR("alloc: vir_addr=0x%x, phy_addr=0x%x, size=%d, flag=0x%x, devAddr=0x%x"), 
					(IM_INT32)mapBuffer.vir_addr, mapBuffer.phy_addr, mapBuffer.size, mapBuffer.flag, devAddr));
				for(i=0; i<size; i+=4){
					*(IM_INT32 *)((IM_INT32)mapBuffer.vir_addr + i) = (IM_INT32)mapBuffer.vir_addr + i;
					tmp = *(IM_INT32 *)((IM_INT32)mapBuffer.vir_addr + i);
					if(tmp != (IM_INT32)mapBuffer.vir_addr + i){
						IM_ERRMSG((IM_STR("read/write vir_addr=0x%x failed"), (IM_INT32)mapBuffer.vir_addr+i));
						goto Fail;
					}
				}
			}

			number++;
		}else{
			length = im_list_size(list);
			if(length != 0){
				if(gTbConfig.mmConfig.randomFree){
					pBuffer = (IM_Buffer *)im_list_get_index(list, get_random_between(0, length-1));
				}else{
					pBuffer = (IM_Buffer *)im_list_begin(list);
				}
				PRINTMSG((IM_STR("free: vir_addr=0x%x, phy_addr=0x%x, size=%d, flag=0x%x"), 
					(IM_INT32)pBuffer->vir_addr, pBuffer->phy_addr, pBuffer->size, pBuffer->flag));

				if(pBuffer->flag & IM_BUFFER_FLAG_MAPPED){
					if(alc_unmap_useraddr(alc, pBuffer) != IM_RET_OK){
						IM_ERRMSG((IM_STR("alc_unmap_useraddr() failed")));
						goto Fail;
					}
					if(gTbConfig.mmConfig.mapAlcMode == MAP_ALC_MODE_MALLOC){
						free(pBuffer->vir_addr);
					}else if(gTbConfig.mmConfig.mapAlcMode == MAP_ALC_MODE_PHY_LINEAR){
						if(alc_free(alc, pBuffer) != IM_RET_OK){
							IM_ERRMSG((IM_STR("alc_free() failed")));
							goto Fail;
						}
					}else if(gTbConfig.mmConfig.mapAlcMode == MAP_ALC_MODE_PHY_NONLINEAR){
						if(alc_free(alc, pBuffer) != IM_RET_OK){
							IM_ERRMSG((IM_STR("alc_free() failed")));
							goto Fail;
						}
					}else{
						free(pBuffer->vir_addr);
					}
				}else{
					if(alc_free(alc, pBuffer) != IM_RET_OK){
						IM_ERRMSG((IM_STR("alc_free() failed")));
						goto Fail;
					}
				}
				im_list_erase(list, (void *)pBuffer);
			}	
		}

		im_oswl_sleep(gTbConfig.mmConfig.delay);
	}while(number < gTbConfig.mmConfig.number);	

	//
	length = im_list_size(list);
	while(length != 0){
		if(gTbConfig.mmConfig.randomFree){
			pBuffer = (IM_Buffer *)im_list_get_index(list, get_random_between(0, length-1));
		}else{
			pBuffer = (IM_Buffer *)im_list_begin(list);
		}
		IM_ASSERT(pBuffer != IM_NULL);
		PRINTMSG((IM_STR("free: vir_addr=0x%x, phy_addr=0x%x, size=%d, flag=0x%x"), 
			(IM_INT32)pBuffer->vir_addr, pBuffer->phy_addr, pBuffer->size, pBuffer->flag));

		if(pBuffer->flag & IM_BUFFER_FLAG_MAPPED){
			if(alc_unmap_useraddr(alc, pBuffer) != IM_RET_OK){
				IM_ERRMSG((IM_STR("alc_unmap_useraddr() failed")));
				goto Fail;
			}
			if(gTbConfig.mmConfig.mapAlcMode == MAP_ALC_MODE_MALLOC){
				free(pBuffer->vir_addr);
			}else if(gTbConfig.mmConfig.mapAlcMode == MAP_ALC_MODE_PHY_LINEAR){
				if(alc_free(alc, pBuffer) != IM_RET_OK){
					IM_ERRMSG((IM_STR("alc_free() failed")));
					goto Fail;
				}
			}else if(gTbConfig.mmConfig.mapAlcMode == MAP_ALC_MODE_PHY_NONLINEAR){
				if(alc_free(alc, pBuffer) != IM_RET_OK){
					IM_ERRMSG((IM_STR("alc_free() failed")));
					goto Fail;
				}
			}else{
				free(pBuffer->vir_addr);
			}
		}else{
			if(alc_free(alc, pBuffer) != IM_RET_OK){
				IM_ERRMSG((IM_STR("alc_free() failed")));
				goto Fail;
			}
		}
		im_list_erase(list, (void *)pBuffer);
		
		im_oswl_sleep(gTbConfig.mmConfig.delay);
		length = im_list_size(list);
	}	

	im_list_deinit(list);
	return IM_RET_OK;
Fail:
	if(list){
		pBuffer = (IM_Buffer *)im_list_begin(list);
		while(pBuffer != IM_NULL){
			if(pBuffer->flag & IM_BUFFER_FLAG_MAPPED){
				alc_unmap_useraddr(alc, pBuffer);
				if(gTbConfig.mmConfig.mapAlcMode == MAP_ALC_MODE_MALLOC){
					free(pBuffer->vir_addr);
				}else if(gTbConfig.mmConfig.mapAlcMode == MAP_ALC_MODE_PHY_LINEAR){
					if(alc_free(alc, pBuffer) != IM_RET_OK){
						IM_ERRMSG((IM_STR("alc_free() failed")));
					}
				}else if(gTbConfig.mmConfig.mapAlcMode == MAP_ALC_MODE_PHY_NONLINEAR){
					if(alc_free(alc, pBuffer) != IM_RET_OK){
						IM_ERRMSG((IM_STR("alc_free() failed")));
					}
				}else{
					free(pBuffer->vir_addr);
				}
			}else{
				alc_free(alc, pBuffer);
			}
			pBuffer = (IM_Buffer *)im_list_erase(list, pBuffer);
		}
		im_list_deinit(list);
	}
	return IM_RET_FAILED;
}

IM_INT32 main(IM_INT32 argc, IM_TCHAR *argv[])
{
	ALCCTX alc;
	IM_TCHAR owner[ALC_OWNER_LEN_MAX];
	IM_TCHAR verString[IM_VERSION_STRING_LEN_MAX + 1];

	//
	if(argc < 2){
		menu_help();
		return 0;
	}

	//
	tb_config_init(argc, argv);

	//
	alc_version(verString);
	IM_INFOMSG((IM_STR("InfotmMediaBuffAlloc version: %s"), verString));

	sprintf(owner, "alct_%d", im_oswl_GetSystemTimeMs());// migrate has problem???
	if(alc_open(&alc, owner) != IM_RET_OK){
		IM_ERRMSG((IM_STR("alc_open() failed")));
		goto Fail;
	}

	//
	if(gTbConfig.func == 0){	// statc
		IM_JIF(statc_test(alc, owner));
	}else if(gTbConfig.func == 1){	// mm
		IM_JIF(mm_test(alc));	
	}

Fail:
	if(alc_close(alc) != IM_RET_OK){
		IM_ERRMSG((IM_STR("alc_close() failed")));
	}
	return 0;
}




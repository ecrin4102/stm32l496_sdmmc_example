/*
 * fileManager.h
 *
 *  Created on: 06 fev. 2020
 *      Author: j.imbert
 */
#ifndef __FILE_MANAGER_H__
#define __FILE_MANAGER_H__

 //#############################################################################
 //                        INCLUDES
 //#############################################################################
#include "global.h"
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
#include <stdarg.h>


//#############################################################################
// 			PUBLIC SYMBOLIC CONSTANTS and MACROS
//#############################################################################
#define	MAX_FILENAME_LENGHT		(255)
#define FILEMANAGER_MAX_LOG_BUFFER_SIZE (1024)
#define MAX_FILE_SIZE			(150000)
#define CONFIG_VERSION						(1)
#define FILE_MANAGER_BINARY_VERSION			(1)
//----------------------------------------------------------------------------
//Definition du root file syst�me => architecture de base des dossiers
#define ROOT_FS_DATAS				"data"
#define	ROOT_FS_DATA_CURRENTS		ROOT_FS_DATAS "/run"
#define	ROOT_FS_DATA_UPLOADABLES	ROOT_FS_DATAS "/uploadables"
#define	ROOT_FS_DATA_ARCHIVES		ROOT_FS_DATAS "/archive"
//-----------------------------------------------------------------------------



//#############################################################################
//            PUBLIC TYPES, STRUCTURES, UNIONS and ENUMS
//#############################################################################
typedef enum {
	FILE_TYPE_CSV = 1,
	FILE_TYPE_BIN,
	FILE_TYPE_LOG,
	FILE_TYPE_JSON,
	FILE_TYPE_UNKNOW
}FILE_TYPE;

typedef struct {
	uint32_t size;
	uint8_t * data;
}FileManagerMeasures_TypeDef;
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

//#############################################################################
//						PUBLIC VARIABLES DECLARATION
//#############################################################################
//-----------------------------------------------------------------------------

//#############################################################################
//                     PUBLIC FUNCTION PROTOTYPES
//#############################################################################
void						FileManagerTask(void * argument);
void						FileManagerInit(void);
void 						FileManagerWriteSpeedTest(void);
void 						FileManagerDeleteBackup(void);
Status_TypeDef				FileManagerCreateRootFs(void);
Status_TypeDef				FileManagerCreateNewWorkingFile(FILE_TYPE type);
void						FileManagerCreateNewWorkingFiles(void);
void 						FileManagerSetBackup(void);
void						FileManagerReadConfigFile(void);
void						FileManagerSaveMeasure(const uint8_t *fileName, const FileManagerMeasures_TypeDef *datas);
void						FileManagerCheckFileSize(uint8_t * filename);
int32_t 					FileManagerGetFileSize(const uint8_t *fileName) ;
void						FileManagerSetFileUploadable(const uint8_t *path);
void						FileManagerArchiveFile(const uint8_t *path);
Status_TypeDef 				FileManagerRefreshWorkingFiles(void);
Status_TypeDef				FileManagerCleanCurrentDir(void);
Status_TypeDef				FileManagerReadFile(const uint8_t *path, uint8_t *buffer, uint32_t start, uint32_t stop, uint32_t *nbReaded, uint32_t timeout);
Status_TypeDef				FileManagerOpenUploadDir(void);
void						FileManagerCloseUploadDir(void);
Status_TypeDef				FileManagerGetNextUploadableFile(uint8_t *path);

//-----------------------------------------------------------------------------
Status_TypeDef				FileManagerDelete(const uint8_t *path);
Status_TypeDef				FileManagerWrite(const uint8_t *path, const uint8_t *data, uint32_t lenght);
FILE_TYPE FileManagerGetFileType(const uint8_t *path);
const uint8_t *				FileManagerGetFileName(const uint8_t *path);
Status_TypeDef				FileManagerRename(const uint8_t *src, const uint8_t *dest);
void						FileManagerLog(const uint8_t *msg);
void						FileManagerGenerateNewName(uint8_t *filename);
//-----------------------------------------------------------------------------

#endif

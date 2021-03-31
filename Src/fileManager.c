
/*
 * fileManager.c
 *
 *  Created on: 06 fev. 2020
 *      Author: j.imbert
 */

 //##############################################################################
 //                        INCLUDES
 //##############################################################################

#include "fileManager.h"
#include "main.h"
#include "taskMessage.h"
#include "appCore.h"
#include "printf.h"
#include "printf.h"
#include "mktime.h"
//----------------------------------------------------------------------------
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "semphr.h"
//----------------------------------------------------------------------------
#include "fatfs.h"
#include "ff.h"
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
#include <stdarg.h>
#include <string.h>
#include <float.h>
#include <stdint.h>

//##############################################################################
//                 EXTERNAL variables, functions
//##############################################################################
extern TaskHandle_t     SuperviserTaskHandle;
extern TaskHandle_t	    FileManagerTaskHandle;
extern QueueHandle_t    FileManagerQueueHandle;
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------



//##############################################################################
//                 PRIVATE SYMBOLIC CONSTANTS and MACROS
//##############################################################################
#define FILE_MANAGER_LOCK(timeout)		xSemaphoreTake(FileManagerMutex, pdMS_TO_TICKS(timeout))
#define FILE_MANAGER_UNLOCK()			xSemaphoreGive(FileManagerMutex)
//##############################################################################
//               PRIVATE TYPES, STRUCTURES, UNIONS and ENUMS
//##############################################################################

//----------------------------------------------------------------------------

//##############################################################################
//                    PRIVATE VARIABLES DEFINITION
//##############################################################################
QueueHandle_t FileManagerMutex;
//----------------------------------------------------------------------------
//TODO : réduir la taille a la taille min
uint8_t 			CurrentDataFileName[MAX_FILENAME_LENGHT];
uint8_t 			CurrentLogFileName[MAX_FILENAME_LENGHT];
int32_t  			CurrentDataFileSize = -1;

DIR	   				UploadDir;
const uint16_t 		ConfigVersion = 1;
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------

//##############################################################################
//                      PRIVATE FUNCTION PROTOTYPE
//##############################################################################
void 	_FileManagerSaveMeasureEvent(FileManagerMeasures_TypeDef * data);
void    _FileManagerCleanQueue(FileManagerMeasures_TypeDef * msg);
void 	_FileManagerInsertBinaryHeader(const uint8_t *fileName);

//##############################################################################
//                       PUBLIC FUNCTION CODE
//##############################################################################

void FileManagerTask(void * argument) {
	TaskMessage_TypeDef  msg;

	//Synchro des inits par le superviseur
	ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
	FileManagerInit();
	vTaskSetApplicationTaskTag( NULL, ( void * ) 1 );

	xTaskNotifyGive(SuperviserTaskHandle);

	while (1) {
		xQueueReceive(FileManagerQueueHandle, &msg, portMAX_DELAY);

		switch (msg.messageType) {
			case MESSAGE_CMD_SAVE_DATA:
				_FileManagerSaveMeasureEvent((FileManagerMeasures_TypeDef *)msg.data);
				FileManagerCheckFileSize(CurrentDataFileName);
				break;
			default:
				_Error_Handler(__FILE__, __LINE__);
				break;
		}

		_FileManagerCleanQueue((FileManagerMeasures_TypeDef *)msg.data);
	}
}

void FileManagerInit(void) {
	//Mutex de protection de la carte SD
	FileManagerMutex = xSemaphoreCreateMutex();
	if (FileManagerMutex == NULL) {
		_Error_Handler(__FILE__, __LINE__);
		return;
	}


	FILE_MANAGER_LOCK(portMAX_DELAY);
	//test carte sd et fatfs
	MX_FATFS_Init();

	FILE_MANAGER_UNLOCK();

	//FileManagerWriteSpeedTest();

	//Création de l'architecture des dossiers si pas présent
	FileManagerCreateRootFs();

	//Fichier actuelle rendu téléchargeable
	FileManagerCleanCurrentDir();

	//Création uniquement du log pour log si erreur dans le fichier de conf
	FileManagerCreateNewWorkingFile(FILE_TYPE_LOG);

	//Création fichier de données
	FileManagerCreateNewWorkingFile( FILE_TYPE_BIN);
}

void FileManagerWriteSpeedTest(void){

	//1Mo
	const uint32_t filesize = 1 << 20;
	const uint32_t buffer_size = 512;
	char *buffer = pvPortMalloc(buffer_size);
	uint32_t i, nbwritted, start, time, speed;
	FIL testFile;
	uint32_t line_error = 0;
	const char content[] = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nam aliquet feugiat justo, sed pellentesque ligula interdum sed. Mauris tempus elit sit amet eleifend luctus. Duis ac blandit justo, a dignissim purus. Donec ultricies blandit nisi, nec aliquet lorem malesuada ac. Etiam tincidunt quam et est venenatis, eu imperdiet risus hendrerit. Vivamus iaculis augue nec lacus porttitor luctus sit amet non nulla. Nunc vitae quam maximus, hendrerit enim at, ullamcorper lacus. Suspendisse sem dolor, tristique ut sed.";

	//Copiage de 512 char
	strcpy(buffer, content);


	FILE_MANAGER_LOCK(portMAX_DELAY);

	f_unlink("test.txt");

	start = HAL_GetTick();

	//HAL_GPIO_WritePin(CMD_LED_ROUGE_Port, CMD_LED_ROUGE_Pin, GPIO_PIN_SET);

	//Ecriture de 100ko
	if(f_open(&testFile, "test.txt", FA_WRITE | FA_READ | FA_CREATE_NEW) != FR_OK){
		_Error_Handler(__FILE__, __LINE__);
		goto CLEAN1;
	}

	for(i=0;i<filesize/buffer_size;i++){
		if(f_write(&testFile, buffer, buffer_size, (UINT* )&nbwritted) != FR_OK){
			_Error_Handler(__FILE__, __LINE__);
			goto CLEAN2;
		}
	}

	//Force l'écriture dans le fichier si ya du cache et réouverture
	f_close(&testFile);

	//Lecture timing sur pin
	//HAL_GPIO_WritePin(CMD_LED_ROUGE_Port, CMD_LED_ROUGE_Pin, GPIO_PIN_RESET);

	time = HAL_GetTick() - start;
	//Speed en ko/s
	speed = filesize / time;

	

	//Réouverture
	if (f_open(&testFile, "test.txt",  FA_READ) != FR_OK) {
		_Error_Handler(__FILE__, __LINE__);
		goto CLEAN1;
	}

	//Vérifie que le fichier est correct
	for(i=0;i<filesize/buffer_size;i++){
		//Supprime le contenu
		memset(buffer, 'a', buffer_size);

		//Lecture du fichier pour vérification
		if(f_read(&testFile, buffer, buffer_size,  (UINT* )&nbwritted) != FR_OK){
			_Error_Handler(__FILE__, __LINE__);
			goto CLEAN2;
		}

		//Compte le nombre de ligne en erreur
		if(strcmp(content, buffer) != 0){
			line_error++;
		}
	}


CLEAN2:
	f_close(&testFile);
	f_unlink("test.txt");

CLEAN1:
	vPortFree(buffer);
	FILE_MANAGER_UNLOCK();

}

void FileManagerCheckFileSize(uint8_t * filename) {
	if (FileManagerGetFileSize(filename) > MAX_FILE_SIZE) {
		FileManagerSetFileUploadable(filename);
		FileManagerCreateNewWorkingFile(FileManagerGetFileType(filename));
	}
}

 void FileManagerSaveMeasureInBinary(const uint8_t *filename, const FileManagerMeasures_TypeDef *data) {
	 uint32_t pos = 0;
	 uint32_t sizeWritted = 0;
	 FIL measureFile;
	 uint8_t buff[1024];
	 uint32_t i;
	 int32_t value;

	 FRESULT res;
	 uint64_t prevTick = 0;
	 uint64_t tick;


	 if (FileManagerGetFileSize(filename) <= 0) {
	 	 _FileManagerInsertBinaryHeader(filename);
	 }


	 FILE_MANAGER_LOCK(portMAX_DELAY);

	 if ((res = f_open(&measureFile, (char *)filename, FA_OPEN_APPEND | FA_WRITE | FA_READ)) != FR_OK) {
		 _Error_Handler(__FILE__, __LINE__);
		 goto CLEAN;
	 }

	 for (i = 0; i < data->size; i++) {
		 value = data->data[i];

		 //Ecriture de la données dans tous les cas
		 memcpy(&buff[pos], &value, sizeof(value));
		 pos += sizeof(value);

		 //Vidage intermédiaire du buffer dans le fichier si plus de place pour une ligne
		 if ( pos >= (sizeof(buff)-1) ) {

			 //Copy 3 "a"
			 strcpy(buff+387, "aaa");

			 if(f_write(&measureFile, (void *)buff, pos-1, (UINT*)&sizeWritted) != FR_OK){
				_Error_Handler(__FILE__, __LINE__);
			 }else{
				 pos -= sizeWritted;
			 }


			 // Check manually with the debugger if is't correct @ address 512
			 f_close(&measureFile);
			 f_open(&measureFile, (char *)filename, FA_OPEN_EXISTING | FA_WRITE | FA_READ);
			 f_read(&measureFile, (void *)buff, 1024, (UINT*)&sizeWritted);
			 f_close(&measureFile);
		 }
	 }

	 //Ecriture de ce qui reste dans le buffer
	 if(f_write(&measureFile, (void *)buff, pos, (UINT*)&sizeWritted) != FR_OK){
		_Error_Handler(__FILE__, __LINE__);
	 }

	 CurrentDataFileSize += sizeWritted;
	 f_close(&measureFile);

 CLEAN:
	 FILE_MANAGER_UNLOCK();
 }


 void _FileManagerInsertBinaryHeader(const uint8_t *filename) {
	 FIL file;
	 //Fake header
	 uint8_t  buff[] = {"Lorem ipsum dolor sit amet, consectetur adipiscing elit. Vivamus egestas lacus vitae libero placerat, aliquam varius sit."};
	 uint16_t size = sizeof(buff);
	 uint32_t nbBytes;

	 FILE_MANAGER_LOCK(portMAX_DELAY);
	 if (f_open(&file, (char *)filename, FA_CREATE_ALWAYS | FA_WRITE | FA_READ) != FR_OK) {
		 goto CLEAN;
	 }

	 f_write(&file, (void *)buff, size, (UINT*)&nbBytes);

 CLEAN:
	 f_close(&file);

	 FILE_MANAGER_UNLOCK();
 }


Status_TypeDef FileManagerCreateRootFs(void) {
	FRESULT res = STATUS_OK;
	FILE_MANAGER_LOCK(portMAX_DELAY);

	res = f_mkdir(ROOT_FS_DATAS);
	if (res != FR_OK && res != FR_EXIST) {
		res = STATUS_ERROR;
		goto CLEAN;
	}

	res = f_mkdir(ROOT_FS_DATA_CURRENTS);
	if (res != FR_OK && res != FR_EXIST) {
		res = STATUS_ERROR;
		goto CLEAN;
	}
	res = f_mkdir(ROOT_FS_DATA_UPLOADABLES);
	if (res != FR_OK && res != FR_EXIST) {
		res = STATUS_ERROR;
		goto CLEAN;
	}
	res = f_mkdir(ROOT_FS_DATA_ARCHIVES);
	if (res != FR_OK && res != FR_EXIST) {
		res = STATUS_ERROR;
		goto CLEAN;
	}

	CLEAN:
	FILE_MANAGER_UNLOCK();
	return(res);
}

Status_TypeDef FileManagerRefreshWorkingFiles(void){
	Status_TypeDef result;

	result = FileManagerCleanCurrentDir();
	if(result != STATUS_OK){
		return(result);
	}

	FileManagerCreateNewWorkingFiles();
	return(result);
}

Status_TypeDef FileManagerCleanCurrentDir(void) {
	DIR		currentDir;
	FILINFO	fileInfo;
	FRESULT	res;
	Status_TypeDef result = STATUS_OK;
	uint8_t *fileName;

	fileName = (uint8_t *)pvPortMalloc(MAX_FILENAME_LENGHT + 1);
	if (fileName == NULL) {
		_Error_Handler(__FILE__, __LINE__);
		return STATUS_ERROR;
	}

	FILE_MANAGER_LOCK(portMAX_DELAY);

	//Vidage du dossier current
	if (f_opendir(&currentDir, ROOT_FS_DATA_CURRENTS) == FR_OK) {
		while (1) {
			res = f_readdir(&currentDir, &fileInfo);
			if (res != FR_OK){
				result = STATUS_ERROR;
				break;
			}

			if (fileInfo.fname[0] == 0) {
				result = STATUS_OK;
				break;
			}

			if (!(fileInfo.fattrib & AM_DIR)) {
				FILE_MANAGER_UNLOCK();
				if(FileManagerGetFileType((uint8_t *)fileInfo.fname) != FILE_TYPE_JSON){
					sprintf((char *)fileName, "%s/%s",ROOT_FS_DATA_CURRENTS,fileInfo.fname);
					FileManagerSetFileUploadable(fileName);
				}
				FILE_MANAGER_LOCK(portMAX_DELAY);
			}
		}
		f_closedir(&currentDir);
	}

	FILE_MANAGER_UNLOCK();
	vPortFree(fileName);
	return(result);
}

 Status_TypeDef FileManagerCreateNewWorkingFile(FILE_TYPE type) {
	 FIL		workingFile;
	 char *p;
	 Status_TypeDef res = STATUS_OK;

	 switch (type) {
		case FILE_TYPE_CSV:
			FileManagerGenerateNewName(CurrentDataFileName);
			strcat((char *)CurrentDataFileName, ".csv");
			p = (char *)CurrentDataFileName;
		break;
		case FILE_TYPE_BIN:
			FileManagerGenerateNewName(CurrentDataFileName);
			strcat((char *)CurrentDataFileName, ".myo");
			p = (char *)CurrentDataFileName;
			CurrentDataFileSize = 0;
			break;
		case FILE_TYPE_LOG:
			FileManagerGenerateNewName(CurrentLogFileName);
			strcat((char *)CurrentLogFileName, ".log");
			p = (char *)CurrentLogFileName;
		break;
		default:
			return(STATUS_ERROR);
	 }

	 FILE_MANAGER_LOCK(portMAX_DELAY);
	 if (f_open(&workingFile, p, FA_WRITE | FA_CREATE_ALWAYS) != FR_OK) {
		 //Todo : Gestion d'erreur nécéssaire
		 res = STATUS_ERROR;
		 goto CLEAN;
	 }
	 f_close(&workingFile);

	 CLEAN:
	 FILE_MANAGER_UNLOCK();
	 return(res);
 }

 void FileManagerCreateNewWorkingFiles(void) {
	FileManagerCreateNewWorkingFile(FILE_TYPE_LOG);
	FileManagerCreateNewWorkingFile(FILE_TYPE_BIN);
 }

 void FileManagerGenerateNewName(uint8_t *filename) {
	 //Fake uid and date
	 sprintf((char *)filename, "%s/%s-20210330", ROOT_FS_DATA_CURRENTS, "01234567");
 }

 int32_t FileManagerGetFileSize(const uint8_t *filename) {
	 FIL file;
	 int32_t size;

	 if(strcmp(filename,CurrentDataFileName) == 0){
		 return(CurrentDataFileSize);
	 }

	 FILE_MANAGER_LOCK(portMAX_DELAY);
	 if (f_open(&file, (char *)filename, FA_READ | FA_OPEN_EXISTING) != FR_OK) {

		 if(f_open(&file, (char *)filename, FA_READ | FA_OPEN_EXISTING) != FR_OK){
			 _Error_Handler(__FILE__, __LINE__);
			 size  = -1;
		 }
		 else {
			 size = f_size(&file);
			 f_close(&file);
		 }
	 }
	 else {
		 size = f_size(&file);
		 f_close(&file);
	 }
	 
	 FILE_MANAGER_UNLOCK();

	 return(size);
 }

 void FileManagerSetFileUploadable(const uint8_t *path) {
	 const uint8_t *filename;
	 char * newPath;
	 
	 //récupération du nom du fichier uniquement et ajout au dossier de transfert
	 //Si pas de sous dossier, alors directement a la racine
	 filename = FileManagerGetFileName(path);
	 if (FileManagerGetFileSize(path) <= 0) {
		 FileManagerArchiveFile(path);
		 return;
	 }

	 //Allocation d'une chaine contenant le nouveau chemin du fichier
	 newPath = pvPortMalloc(strlen((char *)filename) + sizeof(ROOT_FS_DATA_UPLOADABLES) + 1);
	 if (newPath == NULL) {
		 _Error_Handler(__FILE__, __LINE__);
		 return;
	 }

	 //Copie du nouveau chemin
	 sprintf(newPath, "%s/%s", ROOT_FS_DATA_UPLOADABLES, filename);

	 //Protection du fichier
	 //FILE_MANAGER_LOCK(portMAX_DELAY);
	 //Déplacement du fichier vers le nouveau le dossier avant transfert
	 f_rename((char *)path, newPath);
	 //Autorisation d'utiliser la mémoire
	 //FILE_MANAGER_UNLOCK();

	 //Libération de la mémoire
	 vPortFree(newPath);
 }

 //TODO: faire une fonction unique avec FileManagerSetFileUploadable
 void FileManagerArchiveFile(const uint8_t *path) {
	 const uint8_t *filename;

	 //récupération du nom du fichier uniquement et ajout au dossier de transfert
	 //Si pas de sous dossier, alors directement a la racine
	 filename = FileManagerGetFileName(path);

	 //Allocation d'une chaine contenant le nouveau chemin du fichier
	 char * newPath = pvPortMalloc(strlen((char *)filename) + sizeof(ROOT_FS_DATA_ARCHIVES) + 1);
	 if (newPath == NULL) {
		 _Error_Handler(__FILE__, __LINE__);
		 return;
	 }

	 //Copie du nouveau chemin
	 sprintf(newPath, "%s/%s", ROOT_FS_DATA_ARCHIVES, filename);

	 //Protection du fichier
	 FILE_MANAGER_LOCK(portMAX_DELAY);
	 //Supression si fichier même nom
	 f_unlink(newPath);
	 //Déplacement du fichier vers le nouveau dossier
	 f_rename((char *)path, (char *)newPath);
	 //Autorisation d'utiliser la mémoire
	 FILE_MANAGER_UNLOCK();

	 //Libération de la mémoire
	 vPortFree(newPath);
 }


 Status_TypeDef FileManagerReadFile(const uint8_t *path, uint8_t *buffer, uint32_t start, uint32_t stop, uint32_t *nbReaded, uint32_t timeout) {
	 FIL file;
	 Status_TypeDef res = STATUS_OK;
	 *nbReaded = 0;
	 buffer[0] = '\0';

	 if (FILE_MANAGER_LOCK(timeout) != pdTRUE) {
		 return(STATUS_TIME_OUT);
	 }

	 if (f_open(&file, (char *)path, FA_READ ) != FR_OK) {
		 res = STATUS_ERROR;
		 goto END;
	 }

	 if (f_lseek(&file, start) != FR_OK) {
		 res = STATUS_ERROR;
		 goto END2;
	 }

	 if (f_read(&file, (void *)buffer, stop - start-1, (UINT* )nbReaded) != FR_OK) {
		 res = STATUS_ERROR;
		 goto END2;
	 }

	 buffer[*nbReaded] = '\0';

 END2:
	 f_close(&file);
END:
	 FILE_MANAGER_UNLOCK();

	 return(res);
 }
 
 Status_TypeDef FileManagerOpenUploadDir(void) {
	 Status_TypeDef res = STATUS_OK;

	 FILE_MANAGER_LOCK(portMAX_DELAY);

	 if (f_opendir(&UploadDir, ROOT_FS_DATA_UPLOADABLES) != FR_OK) {
		 xSemaphoreGive(FileManagerMutex);
		 res = STATUS_ERROR;
	 }

	 FILE_MANAGER_UNLOCK();
	 return(res);
 }

 void FileManagerCloseUploadDir(void) {
	 FILE_MANAGER_LOCK(portMAX_DELAY);
	 f_closedir(&UploadDir);
	 FILE_MANAGER_UNLOCK();
 }

 Status_TypeDef FileManagerGetNextUploadableFile(uint8_t *path) {
	 FILINFO finfo;
	 
	 FILE_MANAGER_LOCK(portMAX_DELAY);
	 if (f_readdir(&UploadDir, &finfo) != FR_OK) {
		 return(STATUS_ERROR);
	 }
	 FILE_MANAGER_UNLOCK();
	 sprintf((char *)path, "%s/%s", ROOT_FS_DATA_UPLOADABLES, finfo.fname);
	 //Chaine vide si plus de fichier a parcourir
	 if (finfo.fname[0] == '\0') {
		 *path = '\0';
	 }
	 return(STATUS_OK);
 }



 FILE_TYPE FileManagerGetFileType(const uint8_t *path) {
	 char * extension = (strrchr((char *)path, '.')+1);

	 if (strcmp(extension, "csv") == 0) {
		 return(FILE_TYPE_CSV);
	 }else if (strcmp(extension, "myo") == 0) {
		 return(FILE_TYPE_BIN);
	 }else if (strcmp(extension, "log") == 0) {
		 return(FILE_TYPE_LOG);
	 }else if (strcmp(extension, "json") == 0) {
		 return(FILE_TYPE_JSON);
	 }else {
		 return(FILE_TYPE_UNKNOW);
	 }
 }

 Status_TypeDef FileManagerDelete(const uint8_t *path) {
	 Status_TypeDef res = STATUS_OK;
	 if (FILE_MANAGER_LOCK(10000) == pdFALSE) {
		 return(STATUS_TIME_OUT);
	 }

	 if (f_unlink((const TCHAR *)path) != FR_OK) {
		 res = STATUS_ERROR;
	 }

	 FILE_MANAGER_UNLOCK();

	 return(res);
 }

 Status_TypeDef FileManagerWrite(const uint8_t *path, const uint8_t *data, uint32_t lenght) {
	 FIL file;
	 uint32_t nbReaded;
	 Status_TypeDef res = STATUS_OK;

	 if (FILE_MANAGER_LOCK(10000) == pdFALSE) {
		 return(STATUS_TIME_OUT);
	 }

	 if (f_open(&file, (char *)path, FA_WRITE | FA_READ | FA_OPEN_APPEND) != FR_OK) {
		 res = STATUS_ERROR;
		 goto CLEAN1;
	 }
	 
	 if (f_write(&file, (char *)data, lenght, (UINT*)&nbReaded) != FR_OK) {
		 res = STATUS_ERROR;
		 goto CLEAN1;
	 }

	 if (nbReaded != lenght) {
		 res = STATUS_ERROR;
	 }

	CLEAN1:
	 f_close(&file);
	 FILE_MANAGER_UNLOCK();
	 return(res);
 }

 const uint8_t * FileManagerGetFileName(const uint8_t *path) {
	uint8_t * filename;
	if((filename = (uint8_t *)strrchr((char *)path, '\\')) == NULL){
		if((filename = (uint8_t *)strrchr((char *)path, '/')) == NULL){
		 return(path);
		}
	}

	//Se déplace aprés le '/' ou le '\'
	filename++;
	return(filename);
 }

 Status_TypeDef	FileManagerRename(const uint8_t *src, const uint8_t *dest) {
	 FRESULT res = STATUS_OK;
	 FILE_MANAGER_LOCK(portMAX_DELAY);
	 if (f_rename((char *)src, (char *)dest) != FR_OK) {
		 res = STATUS_ERROR;
	 }
	 FILE_MANAGER_UNLOCK();

	 return(res);
 }

 void FileManagerLog(const uint8_t *msg) {
	 FIL logFile;

	 FILE_MANAGER_LOCK(portMAX_DELAY);
	 if (f_open(&logFile, (char *)CurrentLogFileName, FA_OPEN_APPEND | FA_WRITE | FA_READ) != FR_OK) {
		 _Error_Handler(__FILE__, __LINE__);
		 FILE_MANAGER_UNLOCK();
		 return;
	 }

	 f_puts((char *)msg, &logFile);
	 f_close(&logFile);
	 FILE_MANAGER_UNLOCK();
 }


 void _FileManagerSaveMeasureEvent(FileManagerMeasures_TypeDef * data){
	 FileManagerSaveMeasureInBinary(CurrentDataFileName, data);
 }

void _FileManagerCleanQueue(FileManagerMeasures_TypeDef * msg){
	vPortFree(msg->data);
	vPortFree(msg);
}

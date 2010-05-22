#include "IDAClientManager.h"
#include "Configuration.h"
#include "DiffMachine.h"
#include "DataBaseWriter.h"
#include "XGetopt.h"
#include "ProcessUtils.h"
#include "dprintf.h"
#include "DarunGrim.h"

#define strtoul10(X) strtoul(X,NULL,10)
extern int DebugLevel;

int ReadFileInfo(void *arg,int argc,char **argv,char **names)
{
	for(int i=0;i<argc;i++)
	{
		dprintf("%s: %s\n",names[i],argv[i]);
	}
	dprintf("\n");
	return 0;
}

void main(int argc,char *argv[])
{
	bool CreateNewDBFromIDA=TRUE;
	TCHAR *optstring=TEXT("f:i:I:L:ld:S:E:s:e:y");
	int optind=0;
	TCHAR *optarg;
	int c;
	BOOL RetrieveFromFile=FALSE;
	char *TheSourceFilename=NULL;
	char *TheTargetFilename=NULL;
	char *LogFilename=NULL;
	BOOL RetrieveFromDB=FALSE;
	BOOL bListFiles=FALSE;
	char *IDAPath=NULL;
	BOOL UseIDASync=FALSE;

	DWORD StartAddressForSource=0;
	DWORD EndAddressForSource=0;
	DWORD StartAddressForTarget=0;
	DWORD EndAddressForTarget=0;

	int TheSourceFileID;
	int TheTargetFileID;

	while((c=getopt(argc,argv,optstring,&optind,&optarg))!=EOF)
	{
		//_tprintf(TEXT("c=%c optind=%u optarg=%s\n"),c,optind,optarg?optarg:TEXT(""));
		switch(c)
		{
			case 'f':
				RetrieveFromFile=TRUE;
				TheSourceFilename=optarg;
				TheTargetFilename=argv[optind];
				optind++;
				break;
			case 'i':
				RetrieveFromDB=TRUE;
				TheSourceFileID=strtoul10(optarg);
				TheTargetFileID=strtoul10(argv[optind]);
				optind++;
				break;
			case 'd':
				DebugLevel=strtoul10(optarg);
				break;
			case 'S':
				StartAddressForSource=strtoul10(optarg);
				break;
			case 'E':
				EndAddressForSource=strtoul10(optarg);
				break;
			case 's':
				StartAddressForTarget=strtoul10(optarg);
				break;
			case 'e':
				EndAddressForTarget=strtoul10(optarg);
				break;
			case 'I':
				IDAPath=optarg;
				break;
			case 'L':
				LogFilename=optarg;
				break;
			case 'l':
				bListFiles=TRUE;
				break;
			case 'y':
				UseIDASync=TRUE;
				break;
		}
	}
	if(argc<=optind)
	{
		printf("Usage: %s [-f <original filename> <patched filename>]|[-i <original file id> <patched file id>]|-l -L <Log Filename> [-y] <database filename>\r\n\
-f <original filename> <patched filename>\r\n\
	Original filename and patched filename\r\n\
	Retrieve data from IDA using DarunGrim2 IDA plugin\r\n\
-i <original file id> <patched file id>\r\n\
	Original files ID in the database and patched files ID in the database\r\n\
	Retrieve data from database file created using DarunGrim2 IDA plugin\r\n\
-I IDA Program path.\r\n\
-L Debug Log Filename\r\n\
-d <level> Debug Level\r\n\
-S <Start Address> Start Address To Analyze for the Original\r\n\
-E <End Address> Start Address To Analyze for the Original\r\n\
-s <Start Address> Start Address To Analyze for the Patched\r\n\
-e <End Address> Start Address To Analyze for the Patched\r\n\
-y Use IDA synchorinzation mode\r\n\
-l: \r\n\
	List file informations in the <database filename>\r\n\
<database filename>\r\n\
	Database filename to use\r\n\r\n",argv[0]);
		return;
	}

	DarunGrim aDarunGrim;
	if(IDAPath)
		aDarunGrim.SetIDAPath(IDAPath);

	printf("RetrieveFromFile=%d\r\n",RetrieveFromFile);
	printf("RetrieveFromDB=%d\r\n",RetrieveFromDB);
	char *StorageFilename=argv[optind];
	
	if(RetrieveFromFile && TheSourceFilename && TheTargetFilename && StorageFilename)
	{
		aDarunGrim.GenerateDB( StorageFilename, LogFilename, 
			TheSourceFilename,StartAddressForSource,EndAddressForSource,
			TheTargetFilename,StartAddressForTarget,EndAddressForTarget );
	}
	else if( !( RetrieveFromFile || RetrieveFromDB ) )
	{
		aDarunGrim.GenerateDB();
	}

	aDarunGrim.Analyze();

	if(bListFiles)
	{
		//List files information
		//StorageDB.ExecuteStatement(ReadFileInfo,NULL,"SELECT id,OriginalFilePath,ComputerName,UserName,CompanyName,FileVersion,FileDescription,InternalName,ProductName,ModifiedTime,MD5Sum From FileInfo");
	}
}

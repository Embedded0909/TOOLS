//------------------------------------INCLUDE-----------------------------------------
#include <iostream>
#include <Windows.h>
#include <stdio.h>
#include <fstream>

using namespace std;

//--------------------------------------VAR-------------------------------------------
extern "C" {
	typedef int(__stdcall* MYPROC) (UCHAR i[], UCHAR o[]);
}
typedef struct {
	uint16_t time;  
	uint8_t type;   
	uint8_t data;  
} Log_typedef;


typedef struct {
	uint8_t header[4];
	uint8_t cnt;     
	uint8_t mode;    
	Log_typedef log[16];  
} EClogging_typedef;

typedef struct {
	uint8_t hd;
	EClogging_typedef abc;
}ABC;

ABC* check;


UCHAR inputBuff[9];
UCHAR outputBuff[31];
UINT8 mData[9] = {0x43, 0x58, 0x7A, 0x00, 0x00, 0x82, 0x83, 0x00, 0x00};
MYPROC mFunction;

//---------------------------------------API---------------------------------------------

void resetInput();
void resetOutput(UINT8 mX);
void saveToFileBin(UCHAR* data, size_t size, const string& filename);
void saveToFileTxt(const char* filename, const EClogging_typedef* data);
void callSabiFunction();
int loadFunctionDLL();

//---------------------------------------CODE--------------------------------------------

#define SIZE_DATA		70


void resetInput() {
	for (int i = 0; i < 9;i++) {
		inputBuff[i] = 0;
	}
}

void resetOutput(UINT8 mX) {
	for (int i = 0; i < 31;i++) {
		outputBuff[i] = mX;
	}
}

void saveToFileBin(UCHAR* data, size_t size, const string& filename) {
	ofstream file(filename, ios::binary | ios::app);
	if (!file) {
		printf("Error!Check file\n");
		return;
	}
	file.write(reinterpret_cast<const char*>(data), size);
	file.close();
	printf("Write success\n");
}

void saveToFileTxt(const char* filename, const EClogging_typedef* data) {
	FILE* file = NULL;
	if (fopen_s(&file, filename, "w") != 0) {
		printf("Error opening file!\n");
		return;
	}

	fprintf(file, "Header: 0x%08X\n", data->header);
	fprintf(file, "Cnt: 0x%02X\n", data->cnt);
	fprintf(file, "Mode: 0x%02X\n", data->mode);
	fprintf(file, "[\n");

	for (int i = 0; i < 16; i++) {
		fprintf(file, "   {\n");
		fprintf(file, "       Time: 0x%04X\n", data->log[i].time);
		fprintf(file, "       Type: 0x%02X\n", data->log[i].type);
		fprintf(file, "       Data: 0x%02X\n", data->log[i].data);
		fprintf(file, "   }\n");
	}
	fprintf(file, "]\n");

	fclose(file);
	printf("Data saved successfully to %s\n", filename);
}

void callSabiFunction() {
	resetOutput(0x00);
	mFunction(inputBuff, outputBuff);
	printf("Output: \n");
	printf("--------------------------------------------------------------------------------------------\n");
	for (int j = 0; j < 31;j++) {
		printf("%02X ", outputBuff[j]);
	}
	printf("\n");
	printf("--------------------------------------------------------------------------------------------\n");
	saveToFileBin(outputBuff, sizeof(outputBuff), "output.bin");
}

int loadFunctionDLL() {
	resetInput();
	for (int i = 0; i < 9;i++) {
		inputBuff[i] = mData[i];
	}

	HINSTANCE hinstLib = LoadLibrary(L"SabiLib.dll");

	if (hinstLib != NULL) {
		
		auto pdd = (FARPROC)GetProcAddress(HINSTANCE(hinstLib), "BiosCallSabi");	//address function

		if (pdd == NULL) {
			printf("Check name function\n");
		}
		else {
			mFunction = (MYPROC)pdd;
			callSabiFunction();
		}
		FreeLibrary(hinstLib);
	}
	else {
		cout << "Check DLL file" << endl;
	}


	return 0;
}
// TEST FUNCTION
EClogging_typedef mdata = {
	{'A','B','C','D'},
	0x05,
	0x02,
	{
	   {0x1234, 1, 10},
	   {0x0000, 2, 20},
	   {0x1234, 3, 30},
	   {0x1234, 4, 40},
	   {0x1234, 5, 50},
	   {0x1234, 6, 60},
	   {0x1234, 7, 70},
	   {0x1234, 8, 80},
	   {0x1234, 9, 90},
	   {0x1234, 10, 100},
	   {0x1234, 11, 110},
	   {0x1234, 12, 120},
	   {0x1234, 13, 130},
	   {0x1234, 14, 140},
	   {0x1234, 15, 150},
	   {0x1234, 16, 160}
   }
};

uint8_t m[10];

int main()
{
	//loadFunctionDLL();

	uint8_t* ptr = mdata.header;
	ptr += 10;
	for (int i = 0; i < 10; i++) {
		m[i] = *ptr++;
	}
	//saveToFileTxt("output.txt", &mdata);
	return 0;
}


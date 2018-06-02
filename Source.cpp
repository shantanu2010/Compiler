#define _CRT_SECURE_NO_WARNINGS

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef struct intermediateNode{

	int slno;
	int opcode;
	int parameters[6];
	int parameterCount;

}intermediateNode;

typedef struct blockAddress{

	char name[30];
	int address;

}blockAddress;

typedef struct symboltable{

	char symbolName;
	int address;
	int size;

}symbolTable;


symbolTable symboltable[10];
blockAddress block[10];

int memory[300] = { 0 };
int memorySize = 300;

int registers[8] = { 0 };

char operatorCodes[20][10] = { "MOVMR", "MOVRM", "ADD", "SUB", "MUL", "JUMP", "IF", "EQ", "LT", "GT", "LTEQ", "GTEQ", "PRINT", "READ", "ENDIF", "ELSE", "LABEL", "GOTO", "START", "END" };
int operatorCodesSize = 16;

int stack[10];
int top = -1;

int blockIndex = 0;
int maxIntermediate = 0;
int index = 0;



int* registers_interpreter;
int* memory_interpreter;
int labelIndex_interpreter = 0;
int interMediateIndex_interpreter;
int noOfSymbols_interpreter;
blockAddress blockAddress_interpreter[10];
symbolTable symboltable_interpreter[10];
intermediateNode intermedeateLabels_interpreter[50];

void push(int);
int pop();


void intitalizeRegistersInterpretor(){

	registers_interpreter = (int*)malloc(8 * sizeof(int));
}

void initializeMemoryInterpretor(){

	memory_interpreter = (int*)malloc(sizeof(int) * 1000);
}

char getMemoryVariableByIndexInterpretor(int index){

	for (int i = 0; i < noOfSymbols_interpreter; i++){

		if (i == index)
			return symboltable_interpreter[i].symbolName;
	}
	return '\0';
}

void loadMemoryBlockInterpretor(FILE *m){

	int y;
	for (int i = 0; i < 400; i++){

		fscanf(m, "%d", &y);
		memory_interpreter[i] = y;
	}
}
void loadIntermedaiteCodeInterpretor(FILE *ilf, FILE *metadata){

	char *s = (char *)malloc(sizeof(char) * 20);

	fscanf(metadata, "%d", &interMediateIndex_interpreter);
	int i = 0;
	while (i<interMediateIndex_interpreter){

		fscanf(ilf, "%d", &intermedeateLabels_interpreter[i].slno);
		fscanf(ilf, "%d", &intermedeateLabels_interpreter[i].opcode);
		int len = 0;
		fscanf(metadata, "%d", &len);
		intermedeateLabels_interpreter[i].parameterCount = len;
		int j = 0;
		while (j<len){

			fscanf(ilf, "%d", &intermedeateLabels_interpreter[i].parameters[j]);
			j++;
		}
		i++;
	}

}

void readSymbolTableInterpretor(FILE *stf, FILE *metadata, FILE *cfile){

	char sym;
	int add, size, i = 0;
	fscanf(metadata, "%d", &noOfSymbols_interpreter);

	while (i<noOfSymbols_interpreter){
		fscanf(stf, "%c", &sym);
		fscanf(stf, "%d", &add);
		fscanf(stf, "%d", &size);
		symboltable_interpreter[i].symbolName = sym;
		symboltable_interpreter[i].address = add;
		symboltable_interpreter[i].size = size;
		fscanf(stf, "%c", &sym);
		if (size != 0)
			fprintf(cfile, "int %c;\n", symboltable_interpreter[i].symbolName);
		else
			fprintf(cfile, "const int %c;\n", symboltable_interpreter[i].symbolName);
		i++;
	}
}
void readLabelsInterpretor(FILE *metadata){

	fscanf(metadata, "%d", &labelIndex_interpreter);
	for (int i = 0; i < labelIndex_interpreter; i++){

		fscanf(metadata, "%d", &(blockAddress_interpreter[i].address));
		fscanf(metadata, "%s", &(blockAddress_interpreter[i].name));
	}
}
char * get_labelInterpretor(int add){

	for (int i = 0; i < labelIndex_interpreter; i++){

		if (blockAddress_interpreter[i].address == add)
			return blockAddress_interpreter[i].name;
	}
	return "A";
}



char* getRegisterVariableFromNameInterpretor(int index){

	if (index == 0)
		return "AX";
	else if (index == 1)
		return "BX";
	else if (index == 2)
		return "CX";
	else if (index == 3)
		return "DX";
	else if (index == 4)
		return "EX";
	else if (index == 5)
		return "FX";
	else if (index == 6)
		return "GX";
	else if (index == 7)
		return "HX";

	return NULL;
}

void convertToCInterpretor(intermediateNode* interlang, FILE *cfile){


	int i = 0, ifflag = 0;
	char *s, *label = (char *)malloc(sizeof(char) * 10);
	while (i<interMediateIndex_interpreter){


		if (interlang[i].opcode == 1){
			fprintf(cfile, "%c=%s;\n", getMemoryVariableByIndexInterpretor(interlang[i].parameters[0]), getRegisterVariableFromNameInterpretor(interlang[i].parameters[1]));
			i++;
		}
		else if (interlang[i].opcode == 2){
			fprintf(cfile, "%s=%c;\n", getRegisterVariableFromNameInterpretor(interlang[i].parameters[0]), getMemoryVariableByIndexInterpretor(interlang[i].parameters[1]));
			i++;
		}
		else if (interlang[i].opcode == 3){
			fprintf(cfile, "%s=%s+%s;\n", getRegisterVariableFromNameInterpretor(interlang[i].parameters[0]), getRegisterVariableFromNameInterpretor(interlang[i].parameters[1]), getRegisterVariableFromNameInterpretor(interlang[i].parameters[2]));
			i++;
		}
		else if (interlang[i].opcode == 4){
			fprintf(cfile, "%s=%s-%s;\n", getRegisterVariableFromNameInterpretor(interlang[i].parameters[0]), getRegisterVariableFromNameInterpretor(interlang[i].parameters[1]), getRegisterVariableFromNameInterpretor(interlang[i].parameters[2]));
			i++;
		}
		else if (interlang[i].opcode == 5){
			fprintf(cfile, "%s=%s*%s;\n", getRegisterVariableFromNameInterpretor(interlang[i].parameters[0]), getRegisterVariableFromNameInterpretor(interlang[i].parameters[1]), getRegisterVariableFromNameInterpretor(interlang[i].parameters[2]));
			i++;
		}
		else if (interlang[i].opcode == 6){
			s = (char *)malloc(sizeof(char) * 10);
			s = get_labelInterpretor(interlang[i].parameters[0] - 1);
			fprintf(cfile, "goto %s;\n", s);
			i++;
		}
		else if (interlang[i].opcode == 7){

			if (interlang[i].parameters[2] == 8)

				fprintf(cfile, "if(%s == %s){\n", getRegisterVariableFromNameInterpretor(interlang[i].parameters[0]), getRegisterVariableFromNameInterpretor(interlang[i].parameters[1]));

			else if (interlang[i].parameters[2] == 9)

				fprintf(cfile, "if(%s < %s){\n", getRegisterVariableFromNameInterpretor(interlang[i].parameters[0]), getRegisterVariableFromNameInterpretor(interlang[i].parameters[1]));


			else if (interlang[i].parameters[2] == 10)

				fprintf(cfile, "if(%s > %s){\n", getRegisterVariableFromNameInterpretor(interlang[i].parameters[0]), getRegisterVariableFromNameInterpretor(interlang[i].parameters[1]));

			else if (interlang[i].parameters[2] == 11)

				fprintf(cfile, "if(%s <= %s){\n", getRegisterVariableFromNameInterpretor(interlang[i].parameters[0]), getRegisterVariableFromNameInterpretor(interlang[i].parameters[1]));


			else if (interlang[i].parameters[2] == 12)

				fprintf(cfile, "if(%s >= %s){\n", getRegisterVariableFromNameInterpretor(interlang[i].parameters[0]), getRegisterVariableFromNameInterpretor(interlang[i].parameters[1]));
			i++;
		}

		else if (interlang[i].opcode == 13){
			fprintf(cfile, "printf(\"%%d \",%s);\n", getRegisterVariableFromNameInterpretor(interlang[i].parameters[0]));
			i++;

		}
		else if (interlang[i].opcode == 14){
			fprintf(cfile, "scanf(\"%%d\",&%s);\n", getRegisterVariableFromNameInterpretor(interlang[i].parameters[0]));
			i++;
		}
		else if (interlang[i].opcode == 15){
			fprintf(cfile, "\n}\n");
			i++;
		}
		else if (interlang[i].opcode == 16){
			fprintf(cfile, "}\n");
			fprintf(cfile, "else\n");
			fprintf(cfile, "{\n");
			i++;
		}
		else if (interlang[i].opcode == 17){
			fprintf(cfile, "%s:\n", get_labelInterpretor(interlang[i].parameters[0] - 1));
			i++;
		}
	}

	fputs("return 0;\n}\n", cfile);
	fclose(cfile);

}


void loadUnitsInterpretor(FILE* memoryFilePointer, FILE* intermediateFilePointer, FILE*  metadataFilePointer){

	loadMemoryBlockInterpretor(memoryFilePointer);
	loadIntermedaiteCodeInterpretor(intermediateFilePointer, metadataFilePointer);

}


void readUnitsInterpretor(FILE* symbolFilePointer, FILE* metadataFilePointer, FILE* cFilePointer){

	readSymbolTableInterpretor(symbolFilePointer, metadataFilePointer, cFilePointer);
	readLabelsInterpretor(metadataFilePointer);

}




bool checkIsLabel(char* str){

	int len = strlen(str);
	if (str[len - 1] == ':')
		return true;

	return false;

}

int getOperatorCode(char *key){

	if (checkIsLabel(key))
		return 17;

	else {

		for (int i = 0; i < operatorCodesSize; i++){

			if (strcmp(key, operatorCodes[i]) == 0)
				return i + 1;
		}

		return -1;
	}
}
int getArraySize(char *sym){

	int val = 0, i = 0;
	if (strlen(sym) != 1){

		sym += 2;
		while (sym[i] != ']'){

			val = val * 10 + (sym[i] - '0');
			i++;
		}
		return val;
	}
	return 1;
}

int getParameterNumber(char *param){


	if (strcmp(param, "AX") == 0)
		return 0;
	if (strcmp(param, "BX") == 0)
		return 1;
	if (strcmp(param, "CX") == 0)
		return 2;
	if (strcmp(param, "DX") == 0)
		return 3;
	if (strcmp(param, "EX") == 0)
		return 4;
	if (strcmp(param, "FX") == 0)
		return 5;
	if (strcmp(param, "GX") == 0)
		return 6;
	if (strcmp(param, "HX") == 0)
		return 7;

	for (int i = 0; i < index; i++){

		if (param[0] == symboltable[i].symbolName && strlen(param) == 1)
			return symboltable[i].address;
	}

	if (param[strlen(param) - 1] == ']'){

		for (int i = 0; i < index; i++){

			if (param[0] == symboltable[i].symbolName){

				int offset = 0, j = 0;
				param += 2;
				while (param[j] != ']')
					offset = offset * 10 + param[j++] - '0';
				return symboltable[i].address + offset;
			}
		}
	}
	return -1;
}

void displayIntermediateNode(FILE* metadata, FILE* intermediateFilePointer, intermediateNode* il, int nsym){

	fprintf(metadata, "%d ", maxIntermediate);

	for (int i = 0; i < maxIntermediate; i++){

		fprintf(intermediateFilePointer, "%d %d ", il[i].slno, il[i].opcode);

		for (int j = 0; j < il[i].parameterCount; j++){

			fprintf(intermediateFilePointer, "%d ", il[i].parameters[j]);
		}
		fprintf(metadata, "%d ", il[i].parameterCount);

		fprintf(intermediateFilePointer, "\n");

	}

	fprintf(metadata, "%d ", nsym);
	fprintf(metadata, "%d ", blockIndex);

	for (int i = 0; i < blockIndex; i++)
		fprintf(metadata, "%d %s ", (block[i].address) - 1, block[i].name);


}
void displaysymboltable(FILE* symbolTableFilePointer){

	for (int i = 0; i < index; i++)

		fprintf(symbolTableFilePointer, "%c %d %d\n", symboltable[i].symbolName, symboltable[i].address, symboltable[i].size);

	fclose(symbolTableFilePointer);
}
int getBlockAddressFromName(char *lab){

	for (int i = 0; i < blockIndex; i++){

		if (strcmp(block[i].name, lab) == 0)
			return block[i].address;
	}
	return -1;
}



void executeIntermediateCode(intermediateNode* interlang){

	int i = 0, ifflag = 0;
	while (i<maxIntermediate){

		int temp = interlang[i].opcode;

		if (temp == 1){

			memory[interlang[i].parameters[0]] = registers[interlang[i].parameters[1]];
			i++;
		}

		else if (temp == 2){

			memory[interlang[i].parameters[1]] = registers[interlang[i].parameters[0]];
			i++;
		}
		else if (temp == 3){


			registers[interlang[i].parameters[0]] = registers[interlang[i].parameters[1]] + registers[interlang[i].parameters[2]];
			i++;
		}

		else if (temp == 4){

			registers[interlang[i].parameters[0]] = registers[interlang[i].parameters[1]] - registers[interlang[i].parameters[2]];
			i++;
		}
		else if (temp == 5){
			registers[interlang[i].parameters[0]] = registers[interlang[i].parameters[1]] * registers[interlang[i].parameters[2]];
			i++;
		}

		else if (temp == 6)
			i = interlang[i].parameters[0] - 1;

		else if (temp == 7){

			if (interlang[i].parameters[2] == 8){

				if (registers[interlang[i].parameters[0]] == registers[interlang[i].parameters[1]]){

					ifflag = 1;
					i++;
				}
				else
					i = interlang[i].parameters[3] - 1;
			}
			else if (interlang[i].parameters[2] == 9){

				if (registers[interlang[i].parameters[0]] < registers[interlang[i].parameters[1]]){

					ifflag = 1;
					i++;
				}
				else
					i = interlang[i].parameters[3] - 1;
			}
			else if (interlang[i].parameters[2] == 10){

				if (registers[interlang[i].parameters[0]] > registers[interlang[i].parameters[1]]){

					ifflag = 1;
					i++;
				}
				else
					i = interlang[i].parameters[3] - 1;
			}
			else if (interlang[i].parameters[2] == 11){

				if (registers[interlang[i].parameters[0]] <= registers[interlang[i].parameters[1]]){

					ifflag = 1;
					i++;
				}
				else
					i = interlang[i].parameters[3] - 1;
			}
			else if (interlang[i].parameters[2] == 12){

				if (registers[interlang[i].parameters[0]] >= registers[interlang[i].parameters[1]]){

					ifflag = 1;
					i++;
				}
				else
					i = interlang[i].parameters[3] - 1;
			}
		}
		else if (temp == 13){
			printf("%d ", registers[interlang[i].parameters[0]]);
			i++;
		}
		else if (temp == 14){
			printf("\nEnter Data : ");
			scanf("%d", &registers[interlang[i].parameters[0]]);
			i++;
		}

		else if (temp == 16){

			if (ifflag == 1)
				i = interlang[i].parameters[0] - 1;

			else{
				ifflag = 0;
				i++;
			}
		}
		else
			i++;
	}

}

void printMenu(){

	printf("\n\nMENU\n");
	printf("\n 1.Generate Intermediate Code ");
	printf("\n 2.Execute Intermediate Code ");
	printf("\n 3.Generate C File ");
	printf("\n 4.Exit");
	printf("\nEnter your choice(1-4) : ");
}

int main(){


	FILE* inputFilePointer = fopen("input.txt", "r");
	FILE* intermediateFilePointer = fopen("intermediatelang.txt", "w");
	FILE* symbolTableFilePointer = fopen("symboltable.txt", "w");
	FILE* memoryFilePointer = fopen("memory.txt", "w");
	FILE* metadata = fopen("metadata.txt", "w");

	char ch;
	char buf[20];

	int address = 0;
	int nsym = 0;
	bool isDone = false;
	bool isDone2 = false;
	intermediateNode interlang[50];


	int choice;

	do{
		printMenu();
		scanf(" %d", &choice);
		switch (choice){

		case 1:

			fscanf(inputFilePointer, "%s", buf);

			while (strcmp(buf, "DATA") == 0 || strcmp(buf, "CONST") == 0){

				nsym++;
				if (strcmp(buf, "DATA") == 0){

					char buf2[20];
					fscanf(inputFilePointer, "%s", buf2);
					int size = getArraySize(buf2);
					symboltable[index].symbolName = buf2[0];
					symboltable[index].address = address;
					symboltable[index].size = size;
					address += size;
				}
				else if (strcmp(buf, "CONST") == 0){

					char sym;
					int val;
					fgetc(inputFilePointer);
					fscanf(inputFilePointer, "%c", &sym);
					fgetc(inputFilePointer);
					fscanf(inputFilePointer, "%d", &val);
					memory[address] = val;
					symboltable[index].symbolName = sym;
					symboltable[index].address = address;
					symboltable[index].size = 0;
					address += 1;
				}
				fgetc(inputFilePointer);
				fscanf(inputFilePointer, "%s", buf);
				index++;
			}
			printf("\nGeneterating Symbol Table");
			displaysymboltable(symbolTableFilePointer);
			printf("\nGenertaed Symbol Table");
			char lab[20];
			while (fscanf(inputFilePointer, "%s", buf) != '\0' && strcmp(buf, "END")){

				int opcode = getOperatorCode(buf);
				char *param = (char *)malloc(sizeof(char) * 10);
				int j = 0, add;


				if (opcode == 6){

					interlang[maxIntermediate].slno = maxIntermediate + 1;
					interlang[maxIntermediate].opcode = opcode;
					fscanf(inputFilePointer, "%s", lab);
					interlang[maxIntermediate].parameters[0] = getBlockAddressFromName(lab);
					interlang[maxIntermediate].parameterCount = 1;
					maxIntermediate++;

				}
				else if (opcode == 7){

					interlang[maxIntermediate].slno = maxIntermediate + 1;
					interlang[maxIntermediate].opcode = opcode;
					fgetc(inputFilePointer);
					fscanf(inputFilePointer, "%s", param);
					interlang[maxIntermediate].parameters[0] = getParameterNumber(param);
					fscanf(inputFilePointer, "%s", param);
					interlang[maxIntermediate].parameters[2] = getOperatorCode(param);
					fscanf(inputFilePointer, "%s", param);
					interlang[maxIntermediate].parameters[1] = getParameterNumber(param);
					interlang[maxIntermediate].parameterCount = 3;
					push(maxIntermediate);
					fscanf(inputFilePointer, "%s", param);
					maxIntermediate++;
				}

				else if (opcode == 15){
					interlang[maxIntermediate].slno = maxIntermediate + 1;
					interlang[maxIntermediate].opcode = opcode;
					interlang[maxIntermediate].parameterCount = 0;
					add = pop();
					interlang[add].parameters[interlang[add].parameterCount] = maxIntermediate + 1;
					interlang[add].parameterCount++;
					maxIntermediate++;
				}

				else if (opcode == 16){
					interlang[maxIntermediate].slno = maxIntermediate + 1;
					interlang[maxIntermediate].opcode = opcode;
					interlang[maxIntermediate].parameterCount = 0;
					add = pop();
					interlang[add].parameters[interlang[add].parameterCount] = (interlang[maxIntermediate].slno) + 1;
					interlang[add].parameterCount++;
					push(maxIntermediate);
					maxIntermediate++;
				}

				else if (opcode == 17){

					interlang[maxIntermediate].opcode = opcode;
					interlang[maxIntermediate].parameterCount = 1;
					block[blockIndex].address = maxIntermediate + 1;
					interlang[maxIntermediate].parameters[0] = maxIntermediate + 1;
					interlang[maxIntermediate].slno = maxIntermediate + 1;

					buf[strlen(buf) - 1] = '\0';
					strcpy(block[blockIndex].name, buf);
					blockIndex++;
					maxIntermediate++;
				}

				else{
					interlang[maxIntermediate].slno = maxIntermediate + 1;
					interlang[maxIntermediate].opcode = opcode;
					char t = fgetc(inputFilePointer);
					while (t != '\n' && t != '\0'){

						int i = 0;
						t = fgetc(inputFilePointer);
						while (t != ',' && t != '\n' && t != '\0'){
							param[i++] = t;
							t = fgetc(inputFilePointer);
						}
						param[i] = '\0';
						interlang[maxIntermediate].parameters[j++] = getParameterNumber(param);
					}
					interlang[maxIntermediate].parameterCount = j;
					maxIntermediate++;
				}
			}
			printf("\nGenerating Intermediate Code");
			displayIntermediateNode(metadata, intermediateFilePointer, interlang, nsym);
			printf("\nGenerated Intermediate Code");
			fclose(metadata);
			fclose(intermediateFilePointer);

			for (int i = 0; i < memorySize; i++){
				int y;
				y = memory[i];
				fprintf(memoryFilePointer, "%d ", y);


			}
			fclose(memoryFilePointer);
			isDone = true;
			break;
		case 2:
			if (isDone){
				executeIntermediateCode(interlang);
				isDone2 = true;
			}
			else
				printf("\nIntermediate Code Not Generated");
			break;
		case 3:

			if (isDone2){
				FILE* intermediateFilePointer;
				FILE* memoryFilePointer;
				FILE* metadataFilePointer;
				FILE* symbolFilePointer;
				FILE* cFilePointer;

				cFilePointer = fopen("clang1.txt", "w");
				fputs("#include<stdio.h>\n", cFilePointer);
				fputs("int main()\n{\n", cFilePointer);
				fputs("int AX,BX,CX,DX,EX,FX,GX,HX;\n", cFilePointer);


				intermediateFilePointer = fopen("intermediatelang.txt", "r");
				memoryFilePointer = fopen("memory.txt", "r");
				symbolFilePointer = fopen("symboltable.txt", "r");
				metadataFilePointer = fopen("metadata.txt", "r");


				intitalizeRegistersInterpretor();
				initializeMemoryInterpretor();

				loadUnitsInterpretor(memoryFilePointer, intermediateFilePointer, metadataFilePointer);
				readUnitsInterpretor(symbolFilePointer, metadataFilePointer, cFilePointer);

				printf("\nFetching Intermediate Table\n");
				printf("Fetched Intermediate Table\n");
				printf("Fetching Symbol Table\n");
				printf("Fetched Symbol Table\n");
				printf("Fetching MetaData\n");
				printf("Fetched MetaData\n");
				printf("Fetching Memory\n");
				printf("Fetched Memory\n");

				printf("\nConverting into C");
				convertToCInterpretor(intermedeateLabels_interpreter, cFilePointer);
				printf("\nConverted into C");
			}
			else{

				printf("\nIntermediate Code Not Generated");
			}
			break;


		case 4:
			break;
		}
	} while (choice != 4);

	fclose(inputFilePointer);
	printf("\nEnter to Continue : ");
	getchar();
	getchar();
	return 0;
}

//Stack Operations....

void push(int n){

	stack[++top] = n;
}

int pop(){

	int t = stack[top];
	top--;
	return t;
}



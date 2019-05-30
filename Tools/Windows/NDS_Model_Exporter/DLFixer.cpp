/*
**-------------------------------------------------------------------------
**|             | Display List Fixer v0.2 by AntonioND |                  |
**|             ----------------------------------------                  |
**|                                                                       |
**|                                                                       |
**| It removes color commands from a Display list to make lights work.    |
**| Fell free to modify this, but give credit!                            |
**|                                                                       |
**|                                                   Happy coding!!      |
**|                                                                       |
**|                                                                       |
**| Designed for NDS Mesh Converter by PadrinatoR                         |
**-------------------------------------------------------------------------
*/

#include <stdlib.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <conio.h>

//#define DEBUG

unsigned char COMMAND_ARGUMENTS[] = { 0, 0, 0, 0, 0, 0, 0, 0,	//0h
	0, 0, 0, 0, 0, 0, 0, 0,	//8h
	1, 0, 1, 1, 1, 0, 16, 12,	//10h
	16, 12, 9, 3, 3, 0, 0, 0,	//18h
	1, 1, 1, 2, 1, 1, 1, 1,	//20h
	1, 1, 1, 1, 0, 0, 0, 0,	//28h
	1, 1, 1, 1, 32, 0, 0, 0,	//30h
	0, 0, 0, 0, 0, 0, 0, 0,	//38h
	1, 0, 0, 0, 0, 0, 0, 0,	//40h
	0, 0, 0, 0, 0, 0, 0, 0,	//48h
	1, 0, 0, 0, 0, 0, 0, 0,	//50h
	0, 0, 0, 0, 0, 0, 0, 0,	//58h
	1, 0, 0, 0, 0, 0, 0, 0,	//60h
	0, 0, 0, 0, 0, 0, 0, 0,	//68h
	3, 2, 1
};				//70h

unsigned char Command_Arguments[4];
int Arguments;
unsigned char Command[4];
unsigned int FILE_SIZE;

int Color_Command;
/*
#define Start_Command_0			0x00003130
#define Start_Command_1			0x00007FFF
#define Start_Command_2			0x00000000
*/
FILE *ModelFile;
void *Data_Buffer;
unsigned int *Pointer;
unsigned int *Out_Pointer;
unsigned int *Output_File;

char String[512];

void Divide_Commands();
void Debug();

int main(int argc, char *argv[])
{
	//Load file
	ModelFile = fopen("model.bin", "rb+");
	if (ModelFile == NULL) {
		printf("model.bin not found!");
		return (-1);
	}
	fseek(ModelFile, 0, SEEK_END);
	FILE_SIZE = ftell(ModelFile);
	rewind(ModelFile);
	Data_Buffer = (char *)malloc(sizeof(char) * FILE_SIZE);
	fread(Data_Buffer, 1, FILE_SIZE, ModelFile);
	fclose(ModelFile);

	Pointer = (unsigned int *)Data_Buffer;

	//Debug
	if (argc > 1) {
		if ((strcmp((char *)argv[1], "d") * strcmp((char *)argv[1], "D")) == 0)	//:P
		{
			printf("\n\nDebug Mode\n----------\n");
			Color_Command = 0x20;
			getch();
			Debug();
		}
	}

	if (argc > 1) {
		if ((strcmp((char *)argv[1], "c") * strcmp((char *)argv[1], "C")) == 0)	//:P
		{
			printf("\n\nRemove Color\n----------\n");
			Color_Command = 0x20;
		} else if ((strcmp((char *)argv[1], "n") * strcmp((char *)argv[1], "N")) == 0)	//:P
		{
			printf("\n\nRemove Normals\n----------\n");
			Color_Command = 0x21;
		} else {
			printf
			    ("\n\nUse: program.exe [c/n] (Remove color/normals).");
			return -1;
		}
	}

	Out_Pointer = Output_File = (unsigned int *)malloc(FILE_SIZE + 2);

	FILE_SIZE = 0;		//Increases in this loop
	int size = (unsigned int)*Pointer;
	Pointer++;
	Out_Pointer++;

	//Uncomment this and set the apropiates defines if you want to do
	//something else at the start of all your models.
/*
	*Out_Pointer = Start_Command_0; Out_Pointer++; FILE_SIZE++;
	*Out_Pointer = Start_Command_1; Out_Pointer++; FILE_SIZE++;
	*Out_Pointer = Start_Command_2; Out_Pointer++; FILE_SIZE++;
*/
	while (size > 0) {
		Divide_Commands();

		bool colordetected = false;
		bool slotdetected[4];

#ifdef DEBUG
		printf("\n%02x %02x %02x %02x\n", Command[0], Command[1],
		       Command[2], Command[3]);
		getch();
#endif

		//Check commands
		for (int i = 0; i < 4; i++) {
			if (Command[i] == Color_Command) {
				colordetected = true;
				slotdetected[i] = true;
			} else
				slotdetected[i] = false;
		}

		//Fix
		if (colordetected) {
			for (int i = 0; i < 4; i++) {
				if (Command[i] == Color_Command) {
					if (i == 3)
						Command[3] = 0;
					else {
						for (int j = i; j < 3; j++) {
							Command[j] =
							    Command[j + 1];
						}
						Command[3] = 0;
					}
				}
			}
			unsigned int newcommand =
			    (Command[3] << 24) + (Command[2] << 16) +
			    (Command[1] << 8) + (Command[0]);

#ifdef DEBUG
			printf("\n%08x  ->  %08x\n", *Pointer, newcommand);
			getch();
#endif
			*Out_Pointer = newcommand;
			Out_Pointer++;
			FILE_SIZE++;
			Pointer++;
			size--;
			//Save arguments
			for (int i = 0; i < 4; i++) {
				if (slotdetected[i] == false) {
					if (Command_Arguments[i] > 0) {
						for (int j = 0;
						     j < Command_Arguments[i];
						     j++) {
							*Out_Pointer =
							    (unsigned int)
							    *Pointer;
							Out_Pointer++;
							FILE_SIZE++;
							Pointer++;
							size--;
						}
					}
				} else	//FIFO_COLOR has always one argument XD
				{
					Pointer++;
					size--;
				}
			}
		} else {
			*Out_Pointer = (unsigned int)*Pointer;
			Out_Pointer++;
			FILE_SIZE++;
			Pointer++;
			size--;

			for (int i = 0; i < 4; i++) {
				if (Command_Arguments[i] != 0) {
					for (int j = 0;
					     j < Command_Arguments[i]; j++) {
						*Out_Pointer =
						    (unsigned int)*Pointer;
						Out_Pointer++;
						FILE_SIZE++;
						Pointer++;
						size--;
					}
				}
			}

		}

	}

	*Output_File = FILE_SIZE;

	//Create new file
	ModelFile = fopen("model_out.bin", "wb+");
	if (ModelFile == NULL) {
		printf("model_out.bin couldn´t be created!");
		free(Data_Buffer);
		free((void *)Output_File);
		return (-1);
	}
	fwrite(Output_File, 4, FILE_SIZE + 1, ModelFile);
	fclose(ModelFile);

	free(Data_Buffer);
	free((void *)Output_File);

	printf("Done!");

	return 0;
}

void Divide_Commands()
{
	Command[0] = ((unsigned int)*Pointer >> 0) & 0xFF;
	Command[1] = ((unsigned int)*Pointer >> 8) & 0xFF;
	Command[2] = ((unsigned int)*Pointer >> 16) & 0xFF;
	Command[3] = ((unsigned int)*Pointer >> 24) & 0xFF;

	Command_Arguments[0] = COMMAND_ARGUMENTS[Command[0]];
	Command_Arguments[1] = COMMAND_ARGUMENTS[Command[1]];
	Command_Arguments[2] = COMMAND_ARGUMENTS[Command[2]];
	Command_Arguments[3] = COMMAND_ARGUMENTS[Command[3]];

	Arguments =
	    Command_Arguments[0] + Command_Arguments[1] + Command_Arguments[2] +
	    Command_Arguments[3];
}

void Debug()
{
	int size = (unsigned int)*Pointer;
	Pointer++;

	while (size > 0) {
		//Divide commands
		Command[0] = ((unsigned int)*Pointer >> 0) & 0xFF;
		Command[1] = ((unsigned int)*Pointer >> 8) & 0xFF;
		Command[2] = ((unsigned int)*Pointer >> 16) & 0xFF;
		Command[3] = ((unsigned int)*Pointer >> 24) & 0xFF;

		printf("\nCommands:        %02x, %02x, %02x, %02x  (%08x)",
		       Command[0], Command[1], Command[2], Command[3],
		       *Pointer);

		Command_Arguments[0] = COMMAND_ARGUMENTS[Command[0]];
		Command_Arguments[1] = COMMAND_ARGUMENTS[Command[1]];
		Command_Arguments[2] = COMMAND_ARGUMENTS[Command[2]];
		Command_Arguments[3] = COMMAND_ARGUMENTS[Command[3]];
		Arguments =
		    Command_Arguments[0] + Command_Arguments[1] +
		    Command_Arguments[2] + Command_Arguments[3];

		printf("\nArgument Number: %02d, %02d, %02d, %02d  (%d)\n",
		       Command_Arguments[0], Command_Arguments[1],
		       Command_Arguments[2], Command_Arguments[3], Arguments);

		Pointer++;
		size--;

		for (int i = 0; i < 4; i++) {
			if (Command[i] == Color_Command)
				printf("\nColor detected!\n");

			Pointer += Command_Arguments[i];
			size -= Command_Arguments[i];
		}

		getch();
	}
	free(Data_Buffer);
	printf("\n\nEnd!");
	while (1) ;
}

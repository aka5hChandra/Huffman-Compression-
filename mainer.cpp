

//////////////////////////////////////////////////////////////////////////////////////
//	This is the Huffman Coding of text file.										//
//		press 1 to Compress.														//
//		Press 2 to Decompress.														//
//		Press 3 to exit.															//
//																					//
//	Note:1)The Compressed file will be stored with a name".bin" appeneded to the 	//
//		 end of orginal input file name.											//
//		 example, compressing the file named "a.txt" will generate compressed		//
//		 binary file called "a.txt.bin".											//
//		 2)The Decompressed file will stored in defult name "decompressed.txt".		//
//		 3)The Decompressed file will be stored in the folder where the code is 	//
//		   executed.No option is give to select destination folder.                 //
//		 4)Decompressing will overwrite the file named "decompressed.txt" if        //
//		   it is already existing.													//
//		 5)Sample file by name "a.txt" is placed in file folder for demo.			//
//																					//
//////////////////////////////////////////////////////////////////////////////////////

#include "compressor.h"
#include "decompressor.h"
#include <string.h>


int main () 
{

	printf("Huffman Compression\n");
	printf("************************");
	while(true)
	{
		printf("\nEnter\n------------\n 1 to Compress\n 2 to Decompress\n 3 to Exit\n------------\n");
		char input=cin.get();
		char* fileName=(char*)malloc(500);
		char* ch=(char*)malloc(1);

		switch(input)	
		{
		case '1':{
			printf("Enter file name with extension and full path to compress\n");
			gets(ch);
			gets(fileName);
			compress(fileName);
			break;
				 }
		case '2' :
			{
				printf("Enter file name with extension and full path to decompress\n");
				gets(ch);
				gets(fileName);
				decompress(fileName);
				break;
			}
		case '3':
			exit(0);
		default:
			printf("Invalid input try again\n");

		}

	}
}
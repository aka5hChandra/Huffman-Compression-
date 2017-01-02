

//******************************************************************************************************************// 
//																                									//
//																                									//
//		This contains the Code for Decompression of file using Huffman prefix code  	                				//	
//		And all Required Helper Functions.						                									//
//																													//
//																													//
//	The main decompress function																					//
//			void decompress(char* fileName)																			//
//																													//
//	Helper function to reconstruct binary tree for compressed file from its Header data								//
//		Node* reconstructTree(unsigned char* treeData,int length)													//
//																													//
//	Helper function to decompress the file by traversing down the reconstructed binary tree							//			
//		int traversTreeTopDown(Node* tree,Node* root,int* bits,int count,int size,char* output,int* outputCount)	//										//
//																													//
//			                                                                                                        //
//																													//
//******************************************************************************************************************//            						

#include "datatypes.h"

//function declarations
void decompress(char* fileName);
Node* reconstructTree(unsigned char* treeData,int length);
int traversTreeTopDown(Node* tree,Node* root,int* bits,int count,int size,char* output,int* outputCount);
void printPostorder2( Node *tree);

void decompress(char* fileName)
{
	//This is the main Huffman decompressor fucntion, which reconstrct the binary tree
	//from file header and use it to decompress the body of the file

	//Tree to be reconstructed from header details.
	Node* tree=NULL;

	//pointer for file contents.
	char * memblock=NULL;

	int bitCount=0;
	streampos fileSize ;
	//Load contents of compressed file to memory
	ifstream file (fileName, ios::in|ios::binary|ios::ate);
	if (file.is_open())
	{

		fileSize = file.tellg();
		memblock = new char [fileSize];
		file.seekg (0, ios::beg);
		file.read (memblock, fileSize);
		file.close();


		//Allocate memory of input bits to be proccessed
		int* inputBits=(int*)malloc(sizeof(int)*fileSize*8);
		//Allcate memory for Tree data present in header of file
		unsigned char* treeData=(unsigned char*)malloc(sizeof(unsigned char)*fileSize);
		//Variable to store body start position
		int bodyPos=0;
		//flag to indicate header read status
		bool headerRead=false;
		printf("Processing file\n");
		//Process each byte in compressed file
		for(int i=0;i<fileSize;i++)
		{

			int minusThree=-3;

			//Look for -3 i.e is the end of tree data in compressed file.
			if(memblock[i]==minusThree)
			{
				//if -3 was found call for reconstruction of binary tree.
				tree= reconstructTree(treeData,bodyPos);

				//toggle header read flag and break to process body of the file
				headerRead=true;
				break;
			}
			else if(memblock[i]!=minusThree && !headerRead)
			{
				//else if -3 is not found continue storing tree data 
				treeData[i]=memblock[i];
				//increate body start position by one
				bodyPos++;

			}
		}


		//process each byte in body and obtain corresponding bits for them
		for (int i = bodyPos+1; i < fileSize.seekpos(); )
		{
			//variable to count of number of zeros collected so for
			int zeroCount=0;
			//current byte from compressed file
			unsigned char byte=memblock[i];

			unsigned char two=2;

			if(byte==0)
			{


				//if 0 find number of zero from next byte
				i++;
				int numOfZeros=memblock[i];
				//store specifed number of zeros
				for(int k=0;k<numOfZeros;k++)
				{
					inputBits[bitCount++]=0;
				}
				i++;
			}else
			{
				//if not zero obtain byte and get bits of that byte

				unsigned int size = sizeof(unsigned char);
				unsigned int maxPow = 1<<(size*8-1);
				int k=0,j;
				int* bits=new int[8];

				for(;k<size;++k){
					for(j=0;j<8;++j){
						// print last bit and shift left.
						bits[j]=(byte&maxPow? 1 : 0);


						byte = byte<<1;
					}
				}
				//store non zero bits into array
				for(int k=zeroCount;k<8;k++)
				{

					inputBits[bitCount++]=bits[k];

				}
				i++;
			}


		}


		//check if tree is constructed
		if(tree==NULL)
		{
			//if tree is not constructed ,then file is not supported for decompression
			//exit
			printf("Error in Decompression : File type is not supported for decompression.\n ");
			return;
		}
		printf("Tree reconstructed successfully\n");
		//allocate memory of output and output count
		char* output=(char*)malloc(fileSize*8);
		int* outputCount=(int*)malloc(sizeof(int));
		*outputCount=0;

		printf("Decompressing the file using the Binary Tree\n");

		//Decompress the input by traversing down the reconstructed binary tree
		int count=0;
		do{
			count=traversTreeTopDown(tree,tree,inputBits,count,bitCount,output,outputCount);
		}while( count<bitCount);

		//write decompressed data to file 
		char* outFileName="decompresed.txt";
		ofstream ofile (outFileName, std::fstream::out | std::fstream::trunc);
		ofile.write(output,sizeof(char)*(*outputCount));


		/*printf("out put is \n");

		for(int i=0;i<*outputCount;i++)
		{
		std::cout<<output[i];
		}
		printf("\n");
		cin.get();
		*/
		//clean memory
		delete(tree);
		tree=NULL;
		delete(memblock);
		memblock=NULL;
		delete(output);
		output=NULL;
		delete(outputCount);
		outputCount=NULL;
		ofile.close();


		printf("File decompresed successfully.\n");
		printf("Decompresed file is %s\n",outFileName);

	}else
	{
		std::cout<<"Error in Decompression : File could not be opened for decompression "<<std::endl;
		return;
	}




}

Node* reconstructTree(unsigned char* treeData,int length)
{
	//This is the itterative helper function to reconstruct binary tree using header data of compressed file
	printf("Reconstructing binary tree\n");

	//Allocate memory for input nodes
	Node* inputNodes=(node*)malloc(sizeof(node)*length);
	//stack pointer
	int stackTop=0;

	for(int i=0;i<length;i++)
	{

		//create nodes for input tree data with left and right pointers set to NULL
		//and set byte and node type accordingly
		if(treeData[i]!=254)
		{	
			//set type as  0 for non root node and assing input byte value 
			Node *temp=(Node*)malloc(sizeof(Node));
			temp->byte=treeData[i];
			temp->type=0;
			temp->construted=0;
			temp->left=NULL;
			temp->right=NULL;

			//store new node in memory
			inputNodes[stackTop++]=*temp;
			//std::cout<<"construcint left node for "<<inputNodes[stackTop-1].byte<<std::endl;

			//std::cout<<"construcint left node for "<<treeData[i]<<std::endl;

			//delete temp node
			delete temp;
			temp=NULL;

		}else 
		{

			//set type as -1 for  root node  
			Node *temp=(Node*)malloc(sizeof(Node));

			temp->type=-1;
			temp->construted=0;
			temp->left=NULL;
			temp->right=NULL;

			//store new node in memory
			inputNodes[stackTop++]=*temp;
			//delete temp node
			delete temp;
			temp=NULL;

		}


	}


	//stack to hold partial constucted binary tree.
	stack<Node*> nodeStack;

	//constrct tree from created nodes
	for(int i=0;i<stackTop;i++)
	{
		if(inputNodes[i].type==-1){
			//for each root, assing previous two nodes from stack for left and right pointers
			//and set byte to -1 
			Node* temp=(Node*)malloc(sizeof(Node));
			temp->type=1;

			Node* right=nodeStack.top();
			temp->byte=255;

			//assing top node in stack to right pointer 
			temp->right=right;
			nodeStack.pop();

			//assing top node in stack to left pointer 
			Node* left=nodeStack.top();
			temp->left=left;

			//pop top node from stack
			nodeStack.pop();
			//push new node to top of the stack
			nodeStack.push(temp);

		}else
		{
			//for each non root, push node onto stack
			nodeStack.push(&inputNodes[i]);

		}

	}
	//Only remaning node on the top of stack will have reconstucted complete binary tree
	Node* tree= nodeStack.top();


	return tree;
}

int traversTreeTopDown(Node* tree,Node* root,int* bits,int count,int size,char* output,int* outputCount)
{
	//This the helper recurisve function,which travers down the bianry tree
	//using input data and decompress it.

	//process each input bit
	if(bits[count])
	{
		//if input bit is one 
		if( tree->right!=NULL){
			// current right child is not null, recurse with right child
			traversTreeTopDown(tree->right,root,bits,1+count,size,output,outputCount);
		}
		else if(count!=size)
		{ 
			// current right child is null (ie right leaf node is reached)decompress current bit 
			output[*outputCount]=tree->byte;
			(*outputCount)++;
			return count;
		}
	}
	else 
	{
		//if input bit is not one  

		if( tree->left!=NULL){
			// current left child is not null, recurse with left child

			traversTreeTopDown(tree->left,root,bits,1+count,size,output,outputCount);}
		else if(count!=size)
		{
			// current left child is null (ie left leaf node is reached)decompress current bit 
			output[*outputCount]=tree->byte;
			(*outputCount)++;

			return count;

		}
	}
}

void printPostorder2( Node *tree)
{


	if (tree == NULL)
		return;

	// first recur on left subtree
	printPostorder2(tree->left);

	// then recur on right subtree
	printPostorder2(tree->right);


	printf("%x %c %d %d %d\n",tree->byte,tree->byte, tree->frequencySum,tree->prefixCode,tree->height);//,(tree->prefixCode==NULL)?"not yet ":tree->prefixCode);

	// now deal with the node
	//std::cout<<"table "<<std::endl;
	//if(tree->byte<127)	printf("%x %c %d %d %d\n",tree->byte,tree->byte, tree->frequencySum,tree->prefixCode,tree->height);//,(tree->prefixCode==NULL)?"not yet ":tree->prefixCode);
}




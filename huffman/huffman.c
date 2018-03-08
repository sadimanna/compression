#include<stdio.h>
#include<stdlib.h>
struct pixfreq
{
	int pix;
	float freq;
	struct pixfreq *left, *right;
	char code[1];
};
struct huffcode
{
	char code[1];
};
int get_index(int pixval,struct pixfreq pix_freq[])
{
	int i=0;
	while(pixval!=pix_freq[i].pix)
		i++;
	return i;
}
int ismember(int pix,int *done,int k)
{
	int i=0;
	for(i=0;i<k;i++)
	{
		if(pix==done[i])
			return 1;
	}
	return 0;
}
void sort(struct pixfreq pix_freq[],int nnz,int first)
{
	int i,j;
	struct pixfreq temp;
	//Sorting w.r.to probability of occurence
	for(i=0;i<nnz;i++)
	{
		for(j=i+1;j<nnz;j++)
	  {
			if(pix_freq[i].freq < pix_freq[j].freq)
	    {
				temp = pix_freq[i];
				pix_freq[i] = pix_freq[j];
				pix_freq[j] = temp;
	    }
	  }
	}
	//Sorting w.r.to pixel values
	if(first)
	{
		for(i=0;i<nnz;i++)
		{
			for(j=i+1;j<nnz;j++)
	  	  {
					if(pix_freq[i].freq==pix_freq[j].freq && pix_freq[i].pix>pix_freq[j].pix)
	  	    {
						temp = pix_freq[i];
						pix_freq[i] = pix_freq[j];
						pix_freq[j] = temp;
	        }
	    }
		}
	}
}
int strlength(char *str)
{
	int i=0;
	while(*(str+i)!='\0')
		i++;
	return i;
}
void strconcat(char *str, char *parentcode, char add)
{
	int i=0;
	while(*(parentcode+i)!='\0')
	{
		*(str+i)=*(parentcode+i);
		i++;
	}
	if(add!='2')
	{
		str[i]=add;
		str[i+1]='\0';
	}
	else
		str[i]='\0';
}

void get_parent_code(char *childcode, char *parentcode)
{
	int i=0,j=0;
	while(*(childcode+i)!='\0')
		i++;		
	while(*(childcode+j)!='\0' && j!= i-1)
	{
		*(parentcode+j)=*(childcode+j);
		j++;
	}
	parentcode[i-1]='\0';
}


void main()
{
	int i,j;
	char filename[] = "lenagray8x8.bmp";
	int r,c,data=0,offset,hbytes,width,height;
	long bmpsize=0,bmpdataoff=0,bpp=0;
	int **image;
	int temp=0;
	
	//Reading the BMP File
	FILE *image_file;
	image_file = fopen(filename,"rb");
	if(image_file==NULL)
	{
		printf("Error Opening File!!");
		exit(1);
	}
	else
	{
		//Set file position of the stream to the beginning
		printf("Processing BMP Header...\n\n");
		offset = 0;
		fseek(image_file,offset,SEEK_SET);
		printf("Getting file ID...\n\n");
		for(i=0;i<2;i++)
		{
			fread(&data,1,1,image_file);
			printf("%c",data);
		}
		printf("\n\nGetting size of BMP File...\n\n");
		fread(&bmpsize,4,1,image_file);
		printf("Size of the BMP File:: %ld bytes\n\n",bmpsize);
		printf("Getting offset where the pixel arrray starts...\n\n");
		offset = 10;
		fseek(image_file,offset,SEEK_SET);
		fread(&bmpdataoff,4,1,image_file);
		printf("Bitmap data offset:: %ld\n\n",bmpdataoff);
		printf("DIB Header\n\n");
		fread(&hbytes,4,1,image_file);
		printf("Number of bytes in header:: %d\n\n",hbytes);
		fread(&width,4,1,image_file);
		fread(&height,4,1,image_file);
		printf("Width of Image: %d\n",width);
		printf("Height of image: %d\n\n",height);
		fseek(image_file,2,SEEK_CUR);
		fread(&bpp,2,1,image_file);
		printf("Number of bits per pixel: %ld\n\n",bpp);
		printf("Setting offset to start of pixel data...\n\n");
		fseek(image_file,bmpdataoff,SEEK_SET);
		printf("Creating Image array...\n\n");
		image = (int **)malloc(height*sizeof(int *));
		for(i=0;i<height;i++)
		{
			image[i] = (int *)malloc(width*sizeof(int));
		}
		//int image[height][width];
		int numbytes = (bmpsize - bmpdataoff)/3;
		printf("Number of bytes: %d \n\n",numbytes);
		int r,c;
		printf("Reading the BMP File into Image Array...\n\n");
		for(r=0;r<height;r++)
		{
			for(c=0;c<width;c++)
			{
				fread(&temp,3,1,image_file);
				temp = temp&0x0000FF;
				image[r][c] = temp;
			}
		}
		printf("Image array allocated...\n\n");
	}
	//Finding the probability of occurence
	int hist[256];
	for(i=0;i<256;i++)
	{
		hist[i]=0;
	}
	for(i=0;i<height;i++)
		for(j=0;j<width;j++)
			hist[image[i][j]]+=1;
	
	printf("Histogram\n\n");
	//Finding number of non-zero occurences
	int nnz=0;
	for(i=0;i<256;i++)
	{
		if(hist[i]!=0)
			nnz+=1;
	}
	struct pixfreq **pix_freq;
	pix_freq = (struct pixfreq **)malloc(sizeof(struct pixfreq *)*nnz);
	int totpix = height*width;
	pix_freq[0] = (struct pixfreq *)malloc(sizeof(struct pixfreq)*nnz);
	j=0;
	for(i=0;i<256;i++)
	{
		if(hist[i]!=0)
		{
			pix_freq[0][j].pix = i;
			pix_freq[0][j].freq = (float)hist[i]/(float)totpix;
			pix_freq[0][j].left = NULL;
			pix_freq[0][j].right = NULL;
			pix_freq[0][j].code[0] = '\0';
			j++;
		}
	}
	//Sorting the histogram
	sort(pix_freq[0],nnz,1);
	for(i=0;i<nnz;i++)
		printf("%d --> %f\n",pix_freq[0][i].pix,pix_freq[0][i].freq);
	//Building Huffman Tree
	float sumlowest2prob;
	int n=0,k=0,l=0;
	printf("nnz:: %d\n",nnz);
	while(n < nnz-1)
	{
		printf("numNodes:: %d\n",nnz-n);
		//Adding the lowest two probabilities
		sumlowest2prob = pix_freq[n][nnz-n-1].freq+pix_freq[n][nnz-n-2].freq;
		i=0;
		//New position of the combined node
		while(sumlowest2prob<=pix_freq[n][i].freq)
			i++;
		n+=1;
		//Creating new Level
		pix_freq[n] = (struct pixfreq *)malloc(sizeof(struct pixfreq)*(nnz-n));
		for(k=0;k<nnz-n;k++)
		{
			if(k==i)
			{
				pix_freq[n][k].pix=pix_freq[n-1][nnz-n].pix+pix_freq[n-1][nnz-n-1].pix;
				pix_freq[n][k].freq=pix_freq[n-1][nnz-n].freq+pix_freq[n-1][nnz-n-1].freq;
				pix_freq[n][k].left = &pix_freq[n-1][nnz-n-1];
				pix_freq[n][k].right = &pix_freq[n-1][nnz-n];
				pix_freq[n][k].code[0] = '\0';
			}
			else if(k<i)
			{
				pix_freq[n][k] = pix_freq[n-1][k];
			}
			else
			{
				pix_freq[n][k] = pix_freq[n-1][k-1];
			}
		}
		//Sorting the array
		sort(pix_freq[n],nnz-n,0);
		for(i=0;i<nnz-n;i++)
			printf("%d --> %f\n",pix_freq[n][i].pix,pix_freq[n][i].freq);
	}
	//Assigning Code through backtracking
	printf("Assigning Code...\n\n");
	char left = '1';
	char right = '0';
	int breakit=0,index;
	struct huffcode codes[nnz];
	for(j=nnz-1;j>=0;j--)
	{
		for(i=0;i<nnz-j;i++)
		{
			if(pix_freq[j][i].left!=NULL)
			{
				if(pix_freq[j][i].left->code[0]=='\0')
				{
					strconcat(pix_freq[j][i].left->code,pix_freq[j][i].code,left);
				}
				else if(pix_freq[j][i].left->code[0]!='\0' && pix_freq[j][i].code[0]=='\0')
				{
					get_parent_code(pix_freq[j][i].left->code,pix_freq[j][i].code);
				}
				printf("%d -> %s\n",pix_freq[j][i].left->pix,pix_freq[j][i].left->code);
			}
			if(pix_freq[j][i].right!=NULL) 
			{
				if(pix_freq[j][i].right->code[0]=='\0')
				{
					strconcat(pix_freq[j][i].right->code,pix_freq[j][i].code,right);
				}
				else if(pix_freq[j][i].right->code[0]!='\0' && pix_freq[j][i].code[0]=='\0')
				{
					get_parent_code(pix_freq[j][i].right->code,pix_freq[j][i].code);
				}
				printf("%d -> %s\n",pix_freq[j][i].right->pix,pix_freq[j][i].right->code);
			}
			if(pix_freq[j][i].left==NULL || pix_freq[j][i].right==NULL)
			{
				index = get_index(pix_freq[j][i].pix,pix_freq[0]);
				if(pix_freq[0][index].code[0]=='\0')
					strconcat(pix_freq[0][index].code,pix_freq[j][i].code,'2');
			}
		}
	}
	printf("\n\nHuffmann Codes::\n\n");
	printf("pixel values -> Code\n\n");
	for(i=0;i<nnz;i++)
	{
		if(snprintf(NULL,0,"%d",pix_freq[0][i].pix)==2)
			printf("     %d      -> %s\n",pix_freq[0][i].pix,pix_freq[0][i].code);
		else
			printf("    %d      -> %s\n",pix_freq[0][i].pix,pix_freq[0][i].code);
	}
	//Write to text file
	FILE *huffman = fopen("huffmancode.txt","wb");
	fprintf(huffman,"Huffmann Codes::\n\n");
	fprintf(huffman,"pixel values -> Code\n\n");
	for(i=0;i<nnz;i++)
	{
		if(snprintf(NULL,0,"%d",pix_freq[0][i].pix)==2)
			fprintf(huffman,"     %d      -> %s\n",pix_freq[0][i].pix,pix_freq[0][i].code);
		else
			fprintf(huffman,"    %d      -> %s\n",pix_freq[0][i].pix,pix_freq[0][i].code);
	}
	fclose(huffman);
	//Creating Graphical Tree
	FILE *fp = fopen("huffman.dot","wb");
	fprintf(fp,"digraph G {\n");
	int done[nnz*(nnz-1)/2];
	k=0;
	for(j=nnz-1;j>=0;j--)
	{
		for(i=0;i<nnz-j;i++)
		{
			if(j==0)
				fprintf(fp,"%s [label=<%d<BR />%s>]\n",pix_freq[j][i].code,pix_freq[j][i].pix,pix_freq[j][i].code);
			if(j==nnz-1)
				strconcat(pix_freq[j][i].code,"start",'2');
			if(ismember(pix_freq[j][i].pix,done,k)==0)
			{
				if(pix_freq[j][i].left!=NULL)
				{
					fprintf(fp,"%s -> %s [label=%f]\n",pix_freq[j][i].code,pix_freq[j][i].left->code,pix_freq[j][i].left->freq);
				}
				if(pix_freq[j][i].right!=NULL)
				{
					fprintf(fp,"%s -> %s [label=%f]\n",pix_freq[j][i].code,pix_freq[j][i].right->code,pix_freq[j][i].right->freq);
				}
				done[k]=pix_freq[j][i].pix;
				k++;
			}
		}
	}
	fprintf(fp,"}\n");
	fclose(fp);
	//Execute LINUX terminal command to generate png image from dot file
	char commandpng[]="dot huffman.dot -Tpng -o huffman.png";
	system(commandpng);
	char commandpdf[] = "dot huffman.dot -Tpdf -o huffman.pdf";
	system(commandpdf);
}

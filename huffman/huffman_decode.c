#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main()
{
	FILE *encoded_image = fopen("encoded_image.txt","rb");
	FILE *hufftree = fopen("hufftree.dat","rb");
	FILE *image_info = fopen("image_info.dat","rb");
	//Restoring basic information of the Image
	int i,nnz,totalnnz,maxcodelen,height,width;
	fread(&nnz,sizeof(int),1,image_info);
	fread(&height,sizeof(int),1,image_info);
	fread(&width,sizeof(int),1,image_info);
	totalnnz = 2*nnz-1;
	maxcodelen = ceil(log10(2*nnz)/log10(2))+1;	
	//Restoring the Tree	
	//Defining Structures pixfreq
	struct tree
	{
		int pix,larrloc,rarrloc;
		float freq;
		char code[maxcodelen];
	};
	//Declaring the Tree
	struct tree *huffman_tree;
	huffman_tree = (struct tree *)malloc(sizeof(struct tree)*totalnnz);
	int tree_size = sizeof(struct tree);
	for(i=0;i<totalnnz;i++)
		fread(&huffman_tree[i],tree_size,1,hufftree);
	//for(i=0;i<totalnnz;i++)
	//	printf("%d -> %s\n",pix_freq[i].pix,pix_freq[i].code);

	//Reading the BITSTREAM of the Encoded Image
	
	int **huffimage;
	char bit;
	char code[maxcodelen];
	char left = '0';
	char right = '1';
	huffimage = (int **)malloc(height*sizeof(int *));
	for(i=0;i<height;i++)
	{
		huffimage[i] = (int *)malloc(width*sizeof(int));
	}
	int r,c;
	struct tree currnode;
	//currnode = huffman_tree[totalnnz-1];
	printf("Decoding....\n\n");
	for(r=0;r<height;r++)
		for(c=0;c<width;c++)
		{
			currnode = huffman_tree[totalnnz-1];
			while(currnode.larrloc!=-1 && currnode.rarrloc!=-1)
			{
				fscanf(encoded_image,"%c",&bit);
				if(bit==left)
					currnode = huffman_tree[currnode.larrloc];
				else
					currnode = huffman_tree[currnode.rarrloc];
			}
			if(currnode.larrloc==-1 && currnode.rarrloc==-1)
				huffimage[r][c] = currnode.pix;
		}
	/*
	printf("Writing to new Image...\n\n");
	FILE *newImage = fopen("newImage.pgm","wb");
	fprintf(newImage, "P5\n%d %d\n255\n", height, width);
	int tempval;
	for (r=0; r<height; r++)
	{
		for (c=0; c<width; c++)
		{
			tempval = huffimage[r][c];
			fwrite(&tempval,1,1, newImage);  // 0 .. 255
		 }
	}
	printf("Writing to new Image done...\n\n");*/
	FILE *decImage = fopen("decoded_Image2.txt","wb");
	for(r=0;r<height;r++){
		for(c=0;c<width;c++)
			fprintf(decImage,"%d ",huffimage[r][c]);
		fprintf(decImage,"\n");}
	fclose(decImage);
	fclose(encoded_image);
	fclose(hufftree);
	fclose(image_info);
	//fclose(newImage);	
}

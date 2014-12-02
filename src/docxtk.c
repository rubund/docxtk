/*
 * docxtk.c
 *
 * Copyright (C) 2014  Ruben Undheim <ruben.undheim@gmail.com>
 *    
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *   
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *   
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <zip.h>
#include <zipconf.h>

#define BUF_SIZE 4096

char *docxfilename = NULL;
char verbose = 0;
char *outputfilename = NULL;
char *getfilename = NULL;
char *setfilename = NULL;
char *inputfilename = NULL;
FILE *outputfile = NULL;
FILE *inputfile = NULL;

char *getfileinzip(char *zipfile, char *filetofind)
{
	int err = 0;
	struct zip *z = zip_open(zipfile, 0, &err);
	struct zip_stat st;
	zip_stat_init(&st);
	zip_stat(z, filetofind, 0, &st);
	char *contents = (char *)malloc(st.size+1);
	contents[st.size] = 0;
	struct zip_file *f = zip_fopen(z, filetofind, 0);
	zip_fread(f, contents, st.size);
	zip_fclose(f);
	zip_close(z);

	return contents;
}

void replacefileinzip(char *zipfile, char *filename, char *content)
{
	int err = 0;
	struct zip *z = zip_open(zipfile, 0, &err);
	struct zip_stat st;
	zip_stat_init(&st);
	zip_stat(z, filename, 0, &st);
	zip_uint64_t findex = st.index;
	struct zip_source *source = zip_source_buffer(z,content,strlen(content), 0);
	zip_replace(z, findex, source);
	//zip_source_free(source);
	zip_close(z);

}

int copy_file(char *old_filename, char  *new_filename)
{
	FILE  *ptr_old, *ptr_new;
	int  a;

	ptr_old = fopen(old_filename, "r");
	ptr_new = fopen(new_filename, "w");

	while(1) {
		a = fgetc(ptr_old);
		if(!feof(ptr_old))
			fputc(a, ptr_new);
		else
			break;
	}

	fclose(ptr_new);
	fclose(ptr_old);
	return 0;
}

int parse_cmdline(int argc, char **argv)
{
	int s;
	opterr = 0;
	while((s = getopt(argc, argv, "vo:g:s:rwi:")) != -1) {
		switch (s) {
			case 'v':
				verbose = 1;
				break;
			case 'o':
				outputfilename = (char*) malloc(strlen(optarg)+1);
				snprintf(outputfilename,strlen(optarg)+1,"%s",optarg);
				break;
			case 'g':
				getfilename = (char*) malloc(strlen(optarg)+1);
				snprintf(getfilename,strlen(optarg)+1,"%s",optarg);
				break;
			case 's':
				setfilename = (char*) malloc(strlen(optarg)+1);
				snprintf(setfilename,strlen(optarg)+1,"%s",optarg);
				break;
			case 'r':
				getfilename = (char*) malloc(strlen("word/document.xml")+1);
				sprintf(getfilename,"word/document.xml");
				break;
			case 'w':
				setfilename = (char*) malloc(strlen("word/document.xml")+1);
				sprintf(setfilename,"word/document.xml");
				break;
			case 'i':
				inputfilename = (char*) malloc(strlen(optarg)+1);
				snprintf(inputfilename,strlen(optarg)+1,"%s",optarg);
				break;
			case '?':
				if(optopt == 'o')
					fprintf(stderr, "Option -%c requires an argument.\n",optopt);
				else if(optopt == 'g')
					fprintf(stderr, "Option -%c requires an argument.\n",optopt);
				else if(optopt == 's')
					fprintf(stderr, "Option -%c requires an argument.\n",optopt);
				else if(optopt == 'i')
					fprintf(stderr, "Option -%c requires an argument.\n",optopt);
				else if(isprint(optopt)) 
					fprintf(stderr, "Unknown option '-%c'.\n",optopt);
				return -1;
			default:
				abort();
		}
	}

	if(argc != (optind + 1)){
		fprintf(stderr,"Usage: %s <arguments> input.docx\n",argv[0]);
		return -1;
	}
	docxfilename = (char*) malloc(strlen(argv[optind])+1);
	snprintf(docxfilename,strlen(argv[optind])+1,"%s",argv[optind]);
	return 0;
}

int main(int argc, char **argv)
{

	if(parse_cmdline(argc, argv) != 0){
	 	return -1;
	}
	if(outputfile == NULL){
		if(outputfilename != NULL)
			outputfile = fopen(outputfilename, "w");
		else
			outputfile = stdout;
	}
	if(inputfile == NULL){
		if(inputfilename != NULL)
			inputfile = fopen(inputfilename, "r");
		else
			inputfile = stdin;
	}

	if(getfilename != NULL){
		char *contents;
		contents = getfileinzip(docxfilename,getfilename);
		fprintf(outputfile,"%s",contents);
		free(contents);
	}

	// For not editing file in place:
	if(outputfilename != NULL && (setfilename != NULL || 0)){ 
		copy_file(docxfilename,outputfilename);
		free(docxfilename);
		docxfilename = (char*)malloc(strlen(outputfilename)+1);
		memcpy(docxfilename,outputfilename,strlen(outputfilename));
		docxfilename[strlen(outputfilename)] = 0;
	}

	if(setfilename != NULL){
		int sizecounter = 1;
		char *tmpcontent = (char*)malloc(BUF_SIZE);
		char *contents1 = NULL;
		char *contents2 = NULL;
		int totallength = 0;
		int read;
		while ((read = fread((void *)tmpcontent, 1, BUF_SIZE, inputfile))){
			contents2 = (char*)malloc(BUF_SIZE*sizecounter);
			if(contents1 != NULL)
				memcpy(((void*)contents2), ((void*)contents1), BUF_SIZE*(sizecounter-1));
			memcpy(((void*)contents2)+(BUF_SIZE)*(sizecounter-1), ((void*)tmpcontent), BUF_SIZE);
			if(contents1 != NULL)
				free(contents1);
			contents1 = (char*)malloc(BUF_SIZE*sizecounter);
			memcpy(((void*)contents1), ((void*)contents2), BUF_SIZE*sizecounter);
			free(contents2);
			sizecounter++;
			totallength += read;
		}
		free(tmpcontent);
		contents2 = (char*)malloc(totallength+1);
		memcpy(((void*)contents2), ((void*)contents1), totallength);
		free(contents1);
		contents2[totallength] = 0;
		//printf("%s",contents2);
		replacefileinzip(docxfilename,setfilename,contents2);
		free(contents2);
	}

	fclose(outputfile);
	fclose(inputfile);
	return 0;
}


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
#include <zip.h>
#include <zipconf.h>

char *getfileinzip(char *zipfile, char *filetofind){
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

void replacefileinzip(char *zipfile, char *filename, char *content){
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

int main(int argc, char **argv){

	if(argc != 3){
		fprintf(stderr, "Usage: ...\n");
		return -1;
	}

	char *contents;
	contents = getfileinzip(argv[1],argv[2]);
	printf("%s\n",contents);

	replacefileinzip(argv[1],argv[2],contents);

	free(contents);
	return 0;
}


#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define UNION_MODE 0
#define INTERSECT_MODE 1
#define NAME_SIZE  10000

char* concat3(char *ch1, char *ch2, char *ch3)
{
	char *result = (char*)malloc( sizeof(char) * ( strlen(ch1) + strlen(ch2) + strlen(ch3) + 2 ) );
	result = strcpy(result,ch1);
	result = strcat(result,ch2);
	result = strcat(result,ch3);
	return result;
}

void copy_file( char* c1, char* c2, char* dir1, char* dir2, struct dirent* s1, struct dirent* s2, char* sym1 )
{
	//printf("------------------------------------------%s--%s\n",c1,c2);

	if( s1 && s1->d_type == DT_LNK )
	{
		printf("ln -sf %s %s/%s\n",sym1,dir2,s1->d_name);
	}
	else
	{
		if( s2 && s2->d_type == DT_LNK )
		{
			printf("rm %s\n",c2);
		}
		printf("cp -p %s %s\n",c1,dir2);
	}
}

void delete_file( char* c )
{
	printf("rm %s\n",c);
}

void copy_dir(char* c1, char* dir2 )
{
	printf("cp -rp %s %s\n",c1,dir2);
}

void delete_dir(char* c)
{
	printf("rm -rf %s\n",c);
}



void union_dirs( char *dir1, char *dir2, int look )
{
	DIR *dirp1,*dirp2;
	struct dirent *s1,*s2;
	char *c1,*c2;
	int flag=0;
	struct stat *buf1,*buf2;
	char *sym1,*sym2;
	int size1,size2;

	dirp1 = opendir(dir1);

	while( (s1=readdir(dirp1)) )
	{
		if( !strcmp(s1->d_name,".") || !strcmp(s1->d_name,"..") )
			continue;
		dirp2 = opendir(dir2);
		c1 = concat3( dir1, "/" , s1->d_name );

		buf1 = (struct stat*)malloc( sizeof(struct stat) );

		if( s1->d_type == DT_REG )
			stat(c1,buf1);
		else if( s1->d_type == DT_LNK )
		{
			lstat(c1,buf1);

			sym1 = (char*)malloc( (NAME_SIZE+1)*sizeof(char) );
			size1 = readlink(c1,sym1, NAME_SIZE);
			sym1[size1] = 0;
		}

		flag = 0;
		if( s1->d_type == DT_DIR )
		{
			while( (s2=readdir(dirp2)) )
			{

				if( !strcmp(s1->d_name, s2->d_name) )
				{
					if( look )
					{
						flag = 1;
						break;
					}
					c2 = concat3( dir2, "/" , s2->d_name );

					union_dirs(c1,c2,look);

					free(c2);

					flag=1;
					break;
				}
			}
			if(!flag)
			{
				copy_dir(c1,dir2);
			}
		}
		else
		{
			while( (s2=readdir(dirp2)) )
			{

				if( !strcmp(s1->d_name, s2->d_name) )
				{
					if( look )
					{
						flag = 1;
						break;
					}
					c2 = concat3( dir2, "/" , s2->d_name );

					buf2 = (struct stat*)malloc( sizeof(struct stat) );

					if( s2->d_type == DT_REG )
						stat(c2,buf2);
					else
					{
						lstat(c2,buf2);

						sym2 = (char*)malloc( (NAME_SIZE+1)*sizeof(char) );
						size2 = readlink(c2,sym2, NAME_SIZE);
						sym2[size2] = 0;
					}

					if( buf1->st_mtim.tv_sec > buf2->st_mtim.tv_sec )
					{
						copy_file(c1,c2,dir1,dir2,s1,s2,sym1);
					}
					else if( buf1->st_mtim.tv_sec < buf2->st_mtim.tv_sec )
					{
						copy_file(c2,c1,dir2,dir1,s2,s1,sym2);
					}
					else if( s1->d_type == DT_REG && s2->d_type == DT_LNK )
					{
						copy_file(c1,c2,dir1,dir2,s1,s2,sym1);
					}
					else if( s1->d_type == DT_LNK && s2->d_type == DT_REG )
					{
						copy_file(c2,c1,dir2,dir1,s2,s1,sym2);
					}
					else if( buf1->st_size > buf2->st_size )
					{
						copy_file(c1,c2,dir1,dir2,s1,s2,sym1);
					}
					else if( buf1->st_size < buf2->st_size )
					{
						copy_file(c2,c1,dir2,dir1,s2,s1,sym2);
					}


					free(c2);
					if( s2->d_type == DT_LNK )
						free(sym2);
					free(buf2);

					flag=1;
					break;
				}
			}
			if(!flag)
			{
				copy_file(c1,NULL,NULL,dir2,s1,NULL,sym1);
			}
		}

		if( s1->d_type == DT_LNK )
		{
			free(sym1);
		}
		free(c1);
		free(buf1);

		closedir(dirp2);
	}

	closedir(dirp1);

}



void intersect_dirs( char *dir1, char *dir2, int look )
{
	DIR *dirp1,*dirp2;
	struct dirent *s1,*s2;
	char *c1,*c2;
	int flag=0;
	struct stat *buf1,*buf2;
	char *sym1,*sym2;
	int size1,size2;

	dirp1 = opendir(dir1);

	while( (s1=readdir(dirp1)) )
	{
		if( !strcmp(s1->d_name,".") || !strcmp(s1->d_name,"..") )
			continue;
		dirp2 = opendir(dir2);
		c1 = concat3( dir1, "/" , s1->d_name );

		buf1 = (struct stat*)malloc( sizeof(struct stat) );

		if( s1->d_type == DT_REG )
			stat(c1,buf1);
		else if( s1->d_type == DT_LNK )
		{
			lstat(c1,buf1);

			sym1 = (char*)malloc( (NAME_SIZE+1)*sizeof(char) );
			size1 = readlink(c1,sym1, NAME_SIZE);
			sym1[size1] = 0;
		}

		flag = 0;
		if( s1->d_type == DT_DIR )
		{
			while( (s2=readdir(dirp2)) )
			{

				if( !strcmp(s1->d_name, s2->d_name) )
				{
					if( look )
					{
						flag = 1;
						break;
					}
					c2 = concat3( dir2, "/" , s2->d_name );

					intersect_dirs(c1,c2,look);

					free(c2);

					flag=1;
					break;
				}
			}
			if(!flag)
			{
				delete_dir(c1);
			}
		}
		else
		{
			while( (s2=readdir(dirp2)) )
			{

				if( !strcmp(s1->d_name, s2->d_name) )
				{
					if( look )
					{
						flag = 1;
						break;
					}
					c2 = concat3( dir2, "/" , s2->d_name );

					buf2 = (struct stat*)malloc( sizeof(struct stat) );

					if( s2->d_type == DT_REG )
						stat(c2,buf2);
					else
					{
						lstat(c2,buf2);

						sym2 = (char*)malloc( (NAME_SIZE+1)*sizeof(char) );
						size2 = readlink(c2,sym2, NAME_SIZE);
						sym2[size2] = 0;
					}

					if( buf1->st_mtim.tv_sec < buf2->st_mtim.tv_sec )
					{
						copy_file(c1,c2,dir1,dir2,s1,s2,sym1);
					}
					else if( buf1->st_mtim.tv_sec > buf2->st_mtim.tv_sec )
					{
						copy_file(c2,c1,dir2,dir1,s2,s1,sym2);
					}
					else if( s1->d_type == DT_LNK && s2->d_type == DT_REG )
					{
						copy_file(c1,c2,dir1,dir2,s1,s2,sym1);
					}
					else if( s1->d_type == DT_REG && s2->d_type == DT_LNK )
					{
						copy_file(c2,c1,dir2,dir1,s2,s1,sym2);
					}
					else if( buf1->st_size < buf2->st_size )
					{
						copy_file(c1,c2,dir1,dir2,s1,s2,sym1);
					}
					else if( buf1->st_size > buf2->st_size )
					{
						copy_file(c2,c1,dir2,dir1,s2,s1,sym2);
					}


					free(c2);
					if( s2->d_type == DT_LNK )
						free(sym2);
					free(buf2);

					flag=1;
					break;
				}
			}
			if(!flag)
			{
				delete_file(c1);
			}
		}

		if( s1->d_type == DT_LNK )
		{
			free(sym1);
		}
		free(c1);
		free(buf1);

		closedir(dirp2);
	}

	closedir(dirp1);

}


int main(int argc, char **argv)
{
	char *dir1,*dir2;
	int mode = UNION_MODE;
	if( !strcmp(argv[1],"-i") )
	{
		mode = INTERSECT_MODE;
		dir1 = argv[2];
		dir2 = argv[3];
	}
	else
	{
		dir1 = argv[1];
		dir2 = argv[2];
	}

	if( mode == UNION_MODE )
	{
		union_dirs(dir1,dir2,0);
		union_dirs(dir2,dir1,1);
	}
	else
	{
		intersect_dirs(dir1,dir2,0);
		intersect_dirs(dir2,dir1,1);
	}

}

#include<stdio.h>
#define _GNU_SOURCE
#include<unistd.h>
#include<sys/syscall.h>
#define SYS_MAP_LOCK 317
#define SYS_MAP_UNLOCK 318

int main() {
	int x;
	x = syscall(SYS_MAP_LOCK, 0,0,10,10,1);
	printf("%d\n",x);
	x = syscall(SYS_MAP_LOCK, 5,5,15,15,1);
	printf("%d\n",x);
	
}

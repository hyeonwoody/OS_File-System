/*
	ssufs from simplefs
*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <assert.h>

#define BLOCKSIZE 64
#define NUM_BLOCKS 35
#define NUM_DATA_BLOCKS 30
#define NUM_INODE_BLOCKS 4
#define NUM_INODES 8
#define NUM_INODES_PER_BLOCK 2
#define MAX_FILE_SIZE 4 // BLock의 최대 갯수
#define MAX_FILES 8
#define MAX_OPEN_FILES 20
#define MAX_NAME_STRLEN 8
#define INODE_FREE 'x'
#define INODE_IN_USE '1'
#define DATA_BLOCK_FREE 'x'
#define DATA_BLOCK_USED '1'

struct superblock_t
{
	char name[MAX_NAME_STRLEN];
	// 초기화 이후에는 "ssufs"으로 설정된다.
	char inode_freelist[NUM_INODES];
	// inode의 상태를 저장하는 배열로 해당 inode가 사용중일 경우 INODE_IN_USE, 아니면 INODE_FREE로 설정된다.
	char datablock_freelist[NUM_DATA_BLOCKS];
	// DataBlock의 상태를 저장하는 배열로 해당 DataBlock이 사용중일 경우 DATA_BLOCK_USED, 아니면 DATA_BLOCK_FREE로 설정된다.
};

struct inode_t
{
	int status;
	// 해당 inode가 사용중일 경우 INODE_IN_USE, 아니면 INODE_FREE
	char name[MAX_NAME_STRLEN];
	// 파일의 이름
	int file_size;
	// 파일의 사이즈 (bytes)
	int direct_blocks[MAX_FILE_SIZE];
	// 사용중인 Data를 저장할 DataBlock의 인덱스를 저장하며 사용하지 않는 경우 -1
};

struct filehandle_t
{
	int offset;
	// 파일의 현재 offset
	int inode_number;
	// 파일의 inode 번호
};

int open_namei(char *filename);

void ssufs_formatDisk();
int ssufs_allocInode();
void ssufs_freeInode(int inodenum);
void ssufs_readInode(int inodenum, struct inode_t *inodeptr);
void ssufs_writeInode(int inodenum, struct inode_t *inodeptr); 
int ssufs_allocDataBlock();
void ssufs_freeDataBlock(int blocknum);
void ssufs_readDataBlock(int blocknum, char *buf);
void ssufs_writeDataBlock(int blocknum, char *buf);
void ssufs_dump();

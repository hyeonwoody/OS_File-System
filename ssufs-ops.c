#include "ssufs-ops.h"

extern struct filehandle_t file_handle_array[MAX_OPEN_FILES]; // 열린 파일의 목록을 관리한다.

int ssufs_allocFileHandle() {
	/*
		file_handle_array에서 비어있는 첫 인덱스를 반환한다.
	*/
	for(int i = 0; i < MAX_OPEN_FILES; i++) {
		if (file_handle_array[i].inode_number == -1) {
			return i;
		}
	}
	return -1;
}

int ssufs_create(char *filename){
	/*
		filename을 이름으로 갖는 새로운 파일을 파일시스템에 생성한다.

		해당 파일의 존재 여부는 open_namei()를 활용한다.
	*/
	int inodenum;
	struct inode_t *tmp = NULL;

	if (open_namei(filename) != -1)
		return -1;

	if ((inodenum = ssufs_allocInode()) == -1)
		return -1;

	tmp = (struct inode_t *) malloc(sizeof(struct inode_t));

	ssufs_readInode(inodenum, tmp);

	tmp->status = INODE_IN_USE;

	strcpy(tmp->name, filename);
	tmp->file_size = 0;

	ssufs_writeInode(inodenum, tmp);

	free(tmp);
	return inodenum;
}


void ssufs_delete(char *filename){
	/*
		filename에 해당하는 파일을 파일시스템에서 삭제한다.

		해당 파일의 존재 여부는 open_namei()를 활용한다.
	*/
	int inodenum = 0;

	if ((inodenum = open_namei(filename)) != -1)
		ssufs_freeInode(inodenum);
}

int ssufs_open(char *filename){
	/*
		filename에 해당하는 파일을 연다.
		
		해당 파일의 존재 여부는 open_namei()를 활용한다.
		file_handle의 빈 공간은 ssufs_allocFileHandle()를 활용한다.
	*/
	int inodenum;
	int file_handle;

	if ((inodenum = open_namei(filename)) == -1)
		return -1;

	if ((file_handle = ssufs_allocFileHandle()) == -1)
		return -1;

	file_handle_array[file_handle].inode_number = inodenum;
	file_handle_array[file_handle].offset = 0;

	return file_handle;
}

void ssufs_close(int file_handle){
	/*
		file_handle에 있는 파일을 닫는다.
	*/
	file_handle_array[file_handle].inode_number = -1;
	file_handle_array[file_handle].offset = 0;
}

int ssufs_read(int file_handle, char *buf, int nbytes){
	/*
		file_handle의 offset에서 nbytes만큼의 데이터를 buf 배열로 읽어온다.
	*/
	char tempBuf[nbytes];
	int buf_offset = 0;
	int offset = file_handle_array[file_handle].offset;

	struct inode_t *tmp = (struct inode_t *) malloc(sizeof(struct inode_t));
	ssufs_readInode(file_handle_array[file_handle].inode_number, tmp);
	
	int fsize = tmp->file_size;
	offset += nbytes;

	if (fsize == -1) {
		free(tmp);
		return -1;
	}

	if ((offset >= 0) && (offset <= fsize)) {
		offset -= nbytes;

		int startBlockNum = offset / BLOCKSIZE;
		int endBlockNum = (offset + nbytes - 1) / BLOCKSIZE;
		int block_offset = offset % BLOCKSIZE;

		int tmp_bytes = nbytes;
		char tmpBlock[BLOCKSIZE];

		for (int i = startBlockNum; i <= endBlockNum; i++) {
			ssufs_readDataBlock(tmp->direct_blocks[i], tmpBlock);
			memcpy(tempBuf + buf_offset, tmpBlock + block_offset, (block_offset + tmp_bytes) > BLOCKSIZE ? (BLOCKSIZE - block_offset) : tmp_bytes);
			buf_offset += (block_offset + tmp_bytes) > BLOCKSIZE ? (BLOCKSIZE - block_offset) : tmp_bytes;
			tmp_bytes -= (BLOCKSIZE - block_offset);
			block_offset = 0;
		}

		memcpy(buf, tempBuf, nbytes);
		free(tmp);

		ssufs_lseek(file_handle, nbytes);
	
		return 0;
	}
	free(tmp);

	return -1;
}

int ssufs_write(int file_handle, char *buf, int nbytes){
	/*
		buf 배열에서 nbytes만큼의 데이터를 file_handle의 offset로 작성한다.
	*/
	int buf_offset = 0;
	int offset = file_handle_array[file_handle].offset;

	struct inode_t *tmp = (struct inode_t *) malloc(sizeof(struct inode_t));
	ssufs_readInode(file_handle_array[file_handle].inode_number, tmp);

	int startBlockNum = offset / BLOCKSIZE;
	int endBlockNum = (offset + nbytes - 1) / BLOCKSIZE;
	int block_offset = offset % BLOCKSIZE;

	int tmp_bytes = nbytes;
	char *tmpBlock = (char *)calloc(BLOCKSIZE, sizeof(char));

	if ((tmp->file_size + nbytes) > (MAX_FILE_SIZE * BLOCKSIZE)) {
		free(tmp);
		return -1;
	}

	for (int i = startBlockNum; i <= endBlockNum; i++) {
		if (tmp->direct_blocks[i] == -1) {
			if ((tmp->direct_blocks[i] = ssufs_allocDataBlock()) == -1) {
				free(tmp);
				return -1;
			}
			char *newBlock = (char *)calloc(BLOCKSIZE, sizeof(char));
			ssufs_writeDataBlock(tmp->direct_blocks[i], newBlock);
		}
		ssufs_readDataBlock(tmp->direct_blocks[i], tmpBlock);
		memcpy(tmpBlock + block_offset, buf + buf_offset, (block_offset + tmp_bytes) > BLOCKSIZE ? (BLOCKSIZE - block_offset) : tmp_bytes);
		buf_offset += (block_offset + tmp_bytes) > BLOCKSIZE ? (BLOCKSIZE - block_offset) : tmp_bytes;
		tmp_bytes -= (BLOCKSIZE - block_offset);
		block_offset = 0;
		ssufs_writeDataBlock(tmp->direct_blocks[i], tmpBlock);
	}

	tmp->file_size = offset + nbytes;
	ssufs_writeInode(file_handle_array[file_handle].inode_number, tmp);
	
	ssufs_lseek(file_handle, nbytes);
	
	free(tmp);
	return 0;
}

int ssufs_lseek(int file_handle, int nseek){
	/*
		주어진 nseek만큼 file_handle을 증가시킨다.
	*/
	int offset = file_handle_array[file_handle].offset;

	struct inode_t *tmp = (struct inode_t *) malloc(sizeof(struct inode_t));
	ssufs_readInode(file_handle_array[file_handle].inode_number, tmp);
	
	int fsize = tmp->file_size;
	
	if (fsize == -1) {
		free(tmp);
		return -1;
	}

	offset += nseek;

	if ((offset >= 0) && (offset <= fsize)) {
		file_handle_array[file_handle].offset = offset;
		free(tmp);
		return 0;
	}

	free(tmp);
	return -1;
}

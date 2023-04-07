# OS_File-System  
## 숭실대학교 운영체제 2020 가을학기 - 과제6 : 파일 시스템  
  
본 과제의 목표는 데이터를 파일로 가상 디스크에 저장하여 파일 대상으로 다양한 operation을 하는 파일 시스템을 구현하는 것입니다. 
연산의 예로는 파일 만들기, 파일 지우기, 파일 열기, 파일 닫기, 파일 읽기, 파일 쓰기가 있습니다.    

<br>

## Soongsil University. Operating System 2020-2 Assignment$#6 : File System

Implementaion of files system that runs over an emulated disk.

**simplefs_formatDisk()** formats the file system and initializes the superblock.

**simplefs_allocInode()**  allocates a free inode on disk, updates the freelist of inodes and return the number of the inode allocated.

**simplefs freeInode(int inodenum)** frees up the specified inode.

**simplefs readInode(int inodenum, struct inode t * inodeptr)** reads the specified inode from disk and copies.

**simplefs writeInode(int inodenum, struct inode t * inodeptr)** copies the inode from the given pointer onto the disk.

**simplefs allocDataBlock()** allocates a free data block on disk, updates the freelist of blocks.

**simplefs freeDataBlock(int blocknum)** frees up the specified data block.

**simplefs readDataBlock(int blocknum, char * buf)** reads the contents of the specified data block on disk, and copies.

**simplefs writeDataBlock(int blocknum, char * buf)** writes the contents of the specified block from the buffer.

**simplefs dump()** prints the disk state in a readable format.

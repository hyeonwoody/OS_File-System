# OS_File-System
Soongsil University. Operating System 2020-2 Assignment$#6 : File System

Implementaion of files ystem that runs over an emulated disk.

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

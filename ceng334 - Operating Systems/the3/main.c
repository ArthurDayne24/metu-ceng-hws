#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include "ext2.h"

/********************************************************************/

#define BOOT_BLOCK 1024
#define EXT2_BLOCK_SIZE 1024

// utils for bitmap manipultion
#define __NBITS (8 * (int) sizeof (Bitmap))
#define __BMELT(d) ((d) / __NBITS)
#define __BMMASK(d) ((Bitmap) 1 << ((d) % __NBITS))
#define BM_SET(d, set) ((set[__BMELT (d)] |= __BMMASK (d)))
#define BM_CLR(d, set) ((set[__BMELT (d)] &= ~__BMMASK (d)))
#define BM_ISSET(d, set) ((set[__BMELT (d)] & __BMMASK (d)) != 0)

/********************************************************************/

int fd;

struct ext2_super_block super;
struct ext2_group_desc group;

unsigned int block_size, nblockptrs;

// used to keep block numbers majorly hence unsigned int
unsigned int *dummy_block, *dummy_block1, *dummy_block2, *empty_block;

typedef unsigned char Bitmap;
Bitmap *block_bitmap, *inode_bitmap;

char temp_fname[7] = {'f', 'i', 'l', 'e', '0', '1', '\0'};

unsigned int deleted_count;

struct ext2_dir_entry *dir_entry;

struct ext2_inode *lost_found;

unsigned int DIR_ENT_SIZE, lost_found_last_block_idx, lost_found_last_block_number, lost_found_last_occupation;

unsigned int *block2latestInode;

struct inode_dtime {
    unsigned int inode_num;
    unsigned int dtime;
};

struct deleted_file_record {
    char fname[7];
    unsigned int inode_num;
    unsigned int dtime; 
    unsigned int nblocks;
    bool is_recovered;

} deleted_files[120];

/********************************************************************/

void open_image(char* image_name) {

    if ((fd = open(image_name, O_RDWR)) < 0) {
        perror(image_name);
        exit(1);
    }
}

void read_superblock() {

    lseek(fd, BOOT_BLOCK, SEEK_SET);
    read(fd, &super, sizeof(struct ext2_super_block));
}

void derive_block_size_set_relateds() {
    
    // consider super.s_log_block_size
    block_size = EXT2_BLOCK_SIZE << super.s_log_block_size; // by definition

    // alloc memory to keep and indirect block temporarily
    dummy_block = malloc(block_size);
    dummy_block1 = malloc(block_size);
    dummy_block2 = malloc(block_size);

    // block pointers per block
    nblockptrs = block_size / sizeof(unsigned int*); 
}

unsigned int block_offset(unsigned int nblocks) {

    return block_size * (nblocks-1);
}

// read first group descriptor, as only one block group we have
void read_group_descriptor() {

    lseek(fd, BOOT_BLOCK + block_size, SEEK_SET);
    read(fd, &group, sizeof(struct ext2_group_desc));
}

void read_bitmaps() {

    // block bitmap

    block_bitmap = malloc(block_size);

    lseek(fd, BOOT_BLOCK + block_offset(group.bg_block_bitmap), SEEK_SET);
    read(fd, block_bitmap, block_size);

    // inode bitmap

    inode_bitmap = malloc(block_size);

    lseek(fd, BOOT_BLOCK + block_offset(group.bg_inode_bitmap), SEEK_SET);
    read(fd, inode_bitmap, block_size);
}

void register_deleted(char *fname, unsigned int inode_num, unsigned int dtime,
        unsigned int nblocks) {
   
    strncpy(deleted_files[deleted_count].fname, fname, 7);
    deleted_files[deleted_count].inode_num = inode_num;
    deleted_files[deleted_count].dtime = dtime;
    deleted_files[deleted_count].nblocks = nblocks;
    deleted_files[deleted_count].is_recovered = 0;
}

// Return block number corresponding to block-idx-th block of lost_found inode
unsigned int get_block_at_idx(unsigned int block_idx) {

    // get block mapped by block_idx-th entry of inode
    // do appropriate mappings and return block number

    unsigned int block_number;
    
    // direct blocks
    if (block_idx < 12) {
        block_number = lost_found->i_block[block_idx];

        return block_number;
    }
    else {
        fprintf(stderr, "Error at ~140.\n");
        exit(1);
    }
}

void seek_to_inode(unsigned int inode_num) {

    lseek(fd, BOOT_BLOCK + block_offset(group.bg_inode_table) +
            (inode_num-1) * sizeof(struct ext2_inode), SEEK_SET);
}

void set_lost_found_relateds() {

    DIR_ENT_SIZE = sizeof(struct ext2_dir_entry) + 6;

    // padding
    if ((DIR_ENT_SIZE % 4) != 0) {
        DIR_ENT_SIZE += (4 - (DIR_ENT_SIZE % 4));
    }

    dir_entry = malloc(DIR_ENT_SIZE);
    memset(dir_entry, 0, DIR_ENT_SIZE); // for safety

    lost_found = malloc(sizeof(struct ext2_inode));
    seek_to_inode(11);
    read(fd, lost_found, sizeof(struct ext2_inode));

    unsigned int block_number = get_block_at_idx(0);

    lost_found_last_block_idx = 0;
    lost_found_last_block_number = block_number;

    unsigned char *tmp = malloc(12);

    lseek(fd, BOOT_BLOCK + block_offset(lost_found_last_block_number) +
            12, SEEK_SET);
    read(fd, tmp, 12);
    * (unsigned short *) (tmp + sizeof(unsigned int)) = 12;
    lseek(fd, BOOT_BLOCK + block_offset(lost_found_last_block_number) +
            12, SEEK_SET);
    write(fd, tmp, 12);

    free(tmp);

    lost_found_last_occupation = 24;

    // mark rest of last block
    memset(dummy_block, 0, block_size - lost_found_last_occupation);
    * (unsigned short*) (((unsigned char*) dummy_block) + sizeof(unsigned int)) =
        block_size - lost_found_last_occupation;

    lseek(fd, BOOT_BLOCK + block_offset(lost_found_last_block_number) +
            lost_found_last_occupation, SEEK_SET);
    write(fd, dummy_block, block_size - lost_found_last_occupation);
}

unsigned short get_rec_len(unsigned char *block, unsigned int offset_to_dir_ent) {
    return * (unsigned short*) (block + sizeof(unsigned int) + offset_to_dir_ent);

}

void assign_next_dir_entry(struct ext2_dir_entry *dir_entry) {
    
    // lost_found_last_block_idx is not actual block, just like block_idx
    // hence 0 indexed. We retrieve actual block by help of it

    while (1) {

        // If remaining chunk is not sufficient, skip to next block
        if (block_size - lost_found_last_occupation <
                DIR_ENT_SIZE + sizeof(struct ext2_dir_entry)) {

            lost_found_last_block_idx++;
            
            lost_found_last_block_number =
                get_block_at_idx(lost_found_last_block_idx);

            // unexpected case
            if (lost_found_last_block_number == 0) {
                fprintf(stderr, "Unexpected case, probably # recoverable files is >100.\n");
                exit(1);
            } 
            else {
                lseek(fd, BOOT_BLOCK + block_offset(lost_found_last_block_number),
                        SEEK_SET);
                read(fd, dummy_block, block_size);

                unsigned short rec_len = get_rec_len((unsigned char*) dummy_block, 0);

                lost_found_last_occupation = 0;
                
                while (rec_len) {
                    printf("%d\n", rec_len);
                    lost_found_last_occupation += rec_len;

                    if (lost_found_last_occupation == block_size) {
                        break;
                    }

                    rec_len = get_rec_len((unsigned char*) dummy_block,
                            lost_found_last_occupation);
                }

                lost_found_last_occupation = block_size - rec_len;

                continue;
            }
        }

        // write actual data
        lseek(fd, BOOT_BLOCK + block_offset(lost_found_last_block_number) +
                lost_found_last_occupation, SEEK_SET);
        write(fd, dir_entry, DIR_ENT_SIZE);

        lost_found_last_occupation += DIR_ENT_SIZE;

        // mark rest of last block
        memset(dummy_block, 0, block_size - lost_found_last_occupation);
        * (unsigned short*) (((unsigned char*) dummy_block) + sizeof(unsigned int)) =
            block_size - lost_found_last_occupation;

        lseek(fd, BOOT_BLOCK + block_offset(lost_found_last_block_number) +
                lost_found_last_occupation, SEEK_SET);
        write(fd, dummy_block, block_size - lost_found_last_occupation);

        return;
    }

    fprintf(stderr, "Unexpected case, end of maximum size of directory entries.\n");
    exit(1);
}

void find_block_numbers(unsigned int *arr, unsigned int *n,
        struct ext2_inode *inode) {

    unsigned int block_number;

    for (int i = 0; i < 12; i++) {
        block_number = inode->i_block[i];

        if (block_number != 0) {
       
            arr[*n] = block_number;
            *n = (*n + 1);
        }
    }

    // indrect - order 1
    block_number = inode->i_block[12];

    if (block_number != 0) {

        arr[*n] = block_number;
        *n = (*n + 1);

        lseek(fd, BOOT_BLOCK + block_offset(block_number), SEEK_SET);
        read(fd, dummy_block, block_size);

        for (unsigned int i = 0; i < nblockptrs; i++) {
            block_number = dummy_block[i];

            if (block_number != 0) {
           
                arr[*n] = block_number;
                *n = (*n + 1);
            }
        }
    }

    // indrect - order 2
    block_number = inode->i_block[13];
    
    if (block_number != 0) {

        arr[*n] = block_number;
        *n = (*n + 1);

        lseek(fd, BOOT_BLOCK + block_offset(block_number), SEEK_SET);
        read(fd, dummy_block, block_size);
       
        for (unsigned int i = 0; i < nblockptrs; i++) {

            block_number = dummy_block[i];

            if (block_number != 0) {

                arr[*n] = block_number;
                *n = (*n + 1);

                lseek(fd, BOOT_BLOCK + block_offset(block_number), SEEK_SET);
                read(fd, dummy_block1, block_size);

                for (unsigned int j = 0; j < nblockptrs; j++) {

                    block_number = dummy_block1[j];

                    if (block_number != 0) {
                    
                        arr[*n] = block_number;
                        *n = (*n + 1);
                    }
                }
            }
        }
    }

    // indrect - order 3
    block_number = inode->i_block[14];
    
    if (block_number != 0) {

        arr[*n] = block_number;
        *n = (*n + 1);

        lseek(fd, BOOT_BLOCK + block_offset(block_number), SEEK_SET);
        read(fd, dummy_block, block_size);
       
        for (unsigned int i = 0; i < nblockptrs; i++) {

            block_number = dummy_block[i];

            if (block_number != 0) {

                arr[*n] = block_number;
                *n = (*n + 1);

                lseek(fd, BOOT_BLOCK + block_offset(block_number), SEEK_SET);
                read(fd, dummy_block1, block_size);

                for (unsigned int j = 0; j < nblockptrs; j++) {

                    block_number = dummy_block1[j];

                    if (block_number != 0) {

                        arr[*n] = block_number;
                        *n = (*n + 1);

                        lseek(fd, BOOT_BLOCK + block_offset(block_number), SEEK_SET);
                        read(fd, dummy_block2, block_size);

                        for (unsigned int k = 0; k < nblockptrs; k++) {

                            block_number = dummy_block2[k];

                            if (block_number != 0) {
                            
                                arr[*n] = block_number;
                                *n = (*n + 1);
                            }
                        }
                    }
                }
            }
        }
    }
}

int date_ascend_compare(const void *r1, const void *r2) {

    const struct inode_dtime *record1 = (const struct inode_dtime*) r1;
    const struct inode_dtime *record2 = (const struct inode_dtime*) r2;

    return record1->dtime - record2->dtime;
}

int date_descend_compare(const void *r1, const void *r2) {

    const struct deleted_file_record *record1 = (const struct deleted_file_record*) r1;
    const struct deleted_file_record *record2 = (const struct deleted_file_record*) r2;

    return record2->dtime - record1->dtime;
}

int name_compare(const void *r1, const void *r2) {

    const struct deleted_file_record *record1 = (const struct deleted_file_record*) r1;
    const struct deleted_file_record *record2 = (const struct deleted_file_record*) r2;

    return strcmp((const char *) record1->fname, (const char *) record2->fname);
}

void calculate_latest_usages() {

    struct inode_dtime *inode_dtime_arr = malloc(sizeof(struct inode_dtime)
            * (super.s_inodes_per_group+1));

    for (unsigned int inode_num = 1; inode_num <= super.s_inodes_per_group;
            inode_num++) {

        struct ext2_inode inode;

        seek_to_inode(inode_num);
        read(fd, &inode, sizeof(struct ext2_inode));

        inode_dtime_arr[inode_num].inode_num = inode_num;
        inode_dtime_arr[inode_num].dtime = inode.i_dtime;
    }

    qsort(inode_dtime_arr+1, super.s_inodes_per_group, sizeof(struct inode_dtime),
            date_ascend_compare);

    block2latestInode = malloc(sizeof(unsigned int) * (super.s_blocks_count+1));
    memset(block2latestInode, 0, sizeof(unsigned int) * (super.s_blocks_count+1));

    unsigned int *allblocks = malloc(sizeof(unsigned int) * super.s_blocks_count);

    for (unsigned int inode_idx = 1; inode_idx <= super.s_inodes_per_group; inode_idx++) {

        struct ext2_inode inode;

        unsigned int inode_num = inode_dtime_arr[inode_idx].inode_num;

        seek_to_inode(inode_num);
        read(fd, &inode, sizeof(struct ext2_inode));

        unsigned int nallblocks = 0;

        find_block_numbers(allblocks, &nallblocks, &inode);

        for (unsigned int i = 0; i < nallblocks; i++) {
            block2latestInode[allblocks[i]] = inode_num;
        }
    }

    free(allblocks);
    free(inode_dtime_arr);
}

// find deleted files, print them and sort them for further use
void find_deleted_files() {

    deleted_count = 0;

    for (unsigned int inode_num = 12; inode_num <= super.s_inodes_per_group;
            inode_num++) {

        struct ext2_inode inode;

        seek_to_inode(inode_num);
        read(fd, &inode, sizeof(struct ext2_inode));

        if (inode.i_dtime > 0) {

            sprintf(temp_fname+4, "%02d", deleted_count+1);

            register_deleted(temp_fname, inode_num, inode.i_dtime,
                    inode.i_blocks / (block_size / 512));

            deleted_count++;
        }
    }

    for (unsigned int i = 0; i < deleted_count; i++) {

        printf("%s %u %u\n", deleted_files[i].fname, deleted_files[i].dtime,
                deleted_files[i].nblocks);
    }

    printf("###\n");

    qsort(deleted_files, deleted_count, sizeof(struct deleted_file_record),
            date_descend_compare);
}

void recover_files() {

    unsigned int *allblocks = malloc(sizeof(unsigned int) * super.s_blocks_count);
    
    for (unsigned int fidx = 0; fidx < deleted_count; fidx++) {

        struct deleted_file_record *frecord = &deleted_files[fidx];

        unsigned int inode_num = frecord->inode_num;

        struct ext2_inode inode;

        seek_to_inode(inode_num);
        read(fd, &inode, sizeof(struct ext2_inode));

        bool failed = 0;

        unsigned int nallblocks = 0;

        find_block_numbers(allblocks, &nallblocks, &inode);

        for (unsigned int i = 0; i < nallblocks; i++) {

            // problem here ? TODO
            if (inode_num != block2latestInode[allblocks[i]]
            /*        || BM_ISSET(allblocks[i], block_bitmap)*/) {
                failed = 1;
                break;
            }                
        }

        if (failed) {
            continue;
        }

        dir_entry->inode = inode_num;
        dir_entry->name_len = 6;
        dir_entry->file_type = EXT2_FT_REG_FILE;
        dir_entry->rec_len = DIR_ENT_SIZE;
        strncpy(dir_entry->name, frecord->fname, 6);

        assign_next_dir_entry(dir_entry);

        for (unsigned int i = 0; i < nallblocks; i++) {

            BM_SET(allblocks[i], block_bitmap);
            
            block2latestInode[allblocks[i]] = inode_num;
           
            super.s_free_blocks_count--;
            group.bg_free_blocks_count--;
        }

        inode.i_dtime = 0;
        inode.i_links_count = 1;
        inode.i_mode = EXT2_S_IFREG | EXT2_S_IRUSR;

        BM_SET(frecord->inode_num, inode_bitmap);
        super.s_free_inodes_count--;
        group.bg_free_inodes_count--;

        frecord->is_recovered = 1;

        seek_to_inode(inode_num);
        write(fd, &inode, sizeof(struct ext2_inode));
    }

    qsort(deleted_files, deleted_count, sizeof(struct deleted_file_record),
            name_compare);

    for (unsigned int i = 0; i < deleted_count; i++) {

        if (deleted_files[i].is_recovered) {
            printf("%s\n", deleted_files[i].fname);
        }
    }

    free(allblocks);
}

void do_writebacks() {

    // write super block back
    lseek(fd, BOOT_BLOCK, SEEK_SET);
    write(fd, &super, sizeof(struct ext2_super_block));

    // write group descriptor back
    lseek(fd, BOOT_BLOCK + block_size, SEEK_SET);
    write(fd, &group, sizeof(struct ext2_group_desc));

    // write lost+found inode back
    seek_to_inode(11);
    write(fd, lost_found, sizeof(struct ext2_inode));

    // write bitmaps back

    lseek(fd, BOOT_BLOCK + block_offset(group.bg_block_bitmap), SEEK_SET);
    write(fd, block_bitmap, block_size);

    lseek(fd, BOOT_BLOCK + block_offset(group.bg_inode_bitmap), SEEK_SET);
    write(fd, inode_bitmap, block_size);
}

int main(int argc, char *argv[]) {

    if (argc < 2) {
        printf("Provide name of image file.\n");
        exit(1);
    }

    open_image(argv[1]);
    read_superblock();
    derive_block_size_set_relateds();
    read_group_descriptor();
    read_bitmaps();
    find_deleted_files();
    calculate_latest_usages();
    set_lost_found_relateds();
    recover_files();
    do_writebacks();

    // release sources

    close(fd);
    
    free(dummy_block);
    free(dummy_block1);
    free(dummy_block2);
    free(block_bitmap);
    free(inode_bitmap);
    free(dir_entry);
    free(block2latestInode);
    free(lost_found);
    free(empty_block);
    
    return 0;
}


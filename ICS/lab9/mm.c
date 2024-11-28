/*
    name:ZhuHan id:522031910213

    A note for this lab:
    1. At first, I used the implicit free list just like the book does,
    the final score is 45/100.
    2. Then I changed to the explicit free list, the final score is 54/100.
    3. Finally I tried to use the seg list referred by the book.
       The free block is consist of header, pred, succ, and footer.
       I was inspired by the awesomeSE project on github,using 1<<6 as extend size.
       And I use the way to place the small block in the front of the splited block,
       while the large block in the back of the splited block.
       The size 96 is the best choice for this strategy.
       (In fact a range of 80 to 120 will all give a good score 95/100).
    PS. I write a series of check functions,but didn't use them to debug the program.

 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include "mm.h"
#include "memlib.h"

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT - 1)) & ~0x7)

#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

/* Basic constants and macros */
#define WSIZE (sizeof(void *)) /* Word and header/footer size (bytes) */
#define DSIZE (2 * WSIZE)      /* Double word size (bytes) */
#define QSIZE 16               /* Quad word size (bytes) */
#define CHUNKSIZE (1 << 12)    /* Extend heap by this amount (bytes) */

#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define MIN(x, y) ((x) < (y) ? (x) : (y))

/* Pack a size and allocated bit into a word */
#define PACK(size, alloc) ((size) | (alloc))

/* Read and write a word at address p */
#define GET(p) (*(size_t *)(p))
#define PUT(p, val) (*(size_t *)(p) = (val))

/* Get the seg list head of class n */
#define GET_LIST(p) (*(char **)p)

/* Read and write pred and succ pointer at block pointer p */
#define PRED(p) ((char *)p)
#define SUCC(p) ((char *)p + WSIZE)
#define GET_PRED(p) (GET_LIST(p))
#define GET_SUCC(p) (GET_LIST(SUCC(p)))

/* Read the size and allocated fields from address p */
#define GET_SIZE(p) (GET(p) & ~0x7)
#define GET_ALLOC(p) (GET(p) & 0x1)

/* Given block ptr bp, compute address of its header and footer */
#define HDRP(bp) ((char *)(bp)-WSIZE)
#define FTRP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

/* Given block ptr bp, compute address of next and previous blocks */
#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE(((char *)(bp)-WSIZE)))
#define PREV_BLKP(bp) ((char *)(bp)-GET_SIZE(((char *)(bp)-DSIZE)))

/* Segregated list */
#define LIST_HEAD(n) (*(seg_listp + n))
#define LISTSIZE 16 /* Number of seg list */

/* Global variables */
static char *heap_listp = 0; /* Pointer to first block */
static char **seg_listp = 0; /* Pointer to the seg list */

/* Function prototypes for internal helper routines */
static void *extend_heap(size_t words);
static void *coalesce(void *bp);
static void *place(void *bp, size_t asize);
static void delete_block(void *bp);
static void insert_block(void *bp, size_t size);
static void checkblock(void *bp);
static void printblock(void *bp);
static int mm_check(void);
static void mm_checkheap(int verbose);

/*
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
    /* Create the initial empty heap(including the seg lists) */
    if ((seg_listp = mem_sbrk(LISTSIZE * WSIZE)) == (void *)-1)
        return -1;
    /* Initialize the seg lists */
    for (int i = 0; i <= LISTSIZE; i++)
        LIST_HEAD(i) = NULL;
    /* Create the initial empty heap */
    if ((heap_listp = mem_sbrk(4 * WSIZE)) == (void *)-1)
        return -1;
    /* Initialize the free list */
    PUT(heap_listp, PACK(0, 0));                 /* Padding - for alignment 8 bytes */
    PUT(heap_listp + WSIZE, PACK(DSIZE, 1));     /* Prologue header */
    PUT(heap_listp + 2 * WSIZE, PACK(DSIZE, 1)); /* Prologue footer */
    PUT(heap_listp + 3 * WSIZE, PACK(0, 1));     /* Epilogue header */
    heap_listp += 2 * WSIZE;                     /* Move the pointer to the prologue block */
    /* Extend the empty heap with a free block of CHUNKSIZE bytes */
    if (extend_heap((1 << 6) / WSIZE) == NULL)
        return -1;

    return 0;
}

/*
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
    size_t asize;      /* Adjusted block size */
    size_t extendsize; /* Amount to extend heap if no fit */
    char *bp = NULL;

    /* Ignore spurious requests */
    if (size <= 0)
        return NULL;

    /* Adjust block size to include overhead and alignment reqs */
    if (size <= DSIZE)
        asize = 2 * DSIZE;
    else
        asize = DSIZE * ((size + DSIZE + (DSIZE - 1)) / DSIZE);

    /* Search the free list for a fit */
    int index = 0;
    size_t search = asize;

    while (index < LISTSIZE)
    {
        if (((index == LISTSIZE - 1) || (search <= 1)) &&
            (LIST_HEAD(index) != NULL))
        {
            bp = LIST_HEAD(index);

            /** Search the current free list to find the suitable free block */
            while ((bp != NULL) && (GET_SIZE(HDRP(bp))) < asize)
                bp = GET_SUCC(bp);

            /** Found the fit free block */
            if (bp != NULL)
            {
                bp = place(bp, asize);
                return bp;
            }
        }
        search = search >> 1;
        index++;
    }

    /* No fit found. Get more memory and place the block */
    extendsize = MAX(asize, CHUNKSIZE);
    if ((bp = extend_heap(extendsize / WSIZE)) == NULL)
        return NULL;
    bp = place(bp, asize);
    return bp;
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *ptr)
{
    if (!ptr)
        return;

    size_t size = (size_t)GET_SIZE(HDRP(ptr));

    PUT(HDRP(ptr), PACK(size, 0));
    PUT(FTRP(ptr), PACK(size, 0));

    coalesce(ptr);
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
    void *newptr = ptr;

    /** If the bp pointer is NULL, just malloc a block with required size */
    if (ptr == NULL)
    {
        if ((ptr = mm_malloc(size)) == NULL)
            return NULL;
        return ptr;
    }
    /** If realloc size is zero, just free the current block*/
    if (size == 0)
    {
        mm_free(ptr);
        return NULL;
    }

    /** Add the size of Header/Footer for the required payload size */
    if (size <= DSIZE)
        size = 2 * DSIZE;
    /**
     * For requests over 8 bytes:
     * The general rule is to add in the overhead bytes
     * and then round up to the nearest multiple of 8
     */
    else
        size = DSIZE * ((size + (DSIZE) + (DSIZE - 1)) / DSIZE);

    /** Copy the old data from the old one to the new one */
    size_t old_size = (size_t)GET_SIZE(HDRP(ptr));
    if (old_size >= size)
        return ptr;

    /**
     * Otherwise, to check whether
     * the size of combining the current old block the next adjacency block
     * can meet the requirment if the next adjacency is freed.
     * in order to increase the usage of free block
     */

    /** Get the allocate condition for the next block */
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(ptr)));
    size_t next_blk_size = GET_SIZE(HDRP(NEXT_BLKP(ptr)));

    /**
     * When next block is free and
     * the size of two block is greater than or equal the required size,
     * then just combine this two block
     */
    if ((!next_alloc || !next_blk_size) && ((old_size + next_blk_size) >= size))
    {
        delete_block(NEXT_BLKP(ptr));
        PUT(HDRP(ptr), PACK(old_size + next_blk_size, 1));
        PUT(FTRP(ptr), PACK(old_size + next_blk_size, 1));
        return ptr;
    }

    /** Otherwise, allocate newly one */
    if ((newptr = mm_malloc(size)) == NULL)
        return NULL;

    /** Copy the contend of the old block */
    memcpy(newptr, ptr, MIN(size, old_size));
    /** Free the old block */
    mm_free(ptr);
    return newptr;
}

/*
 * extend_heap - Extend the heap with a new free block
 */
static void *extend_heap(size_t words)
{
    char *bp;
    size_t size;

    /* Allocate an even number of words to maintain alignment */
    size = (words % 2) ? (words + 1) * WSIZE : words * WSIZE;
    if ((bp = mem_sbrk(size)) == (void *)-1)
        return NULL;

    /* Initialize free block header/footer and the epilogue header */
    PUT(HDRP(bp), PACK(size, 0));         /* Free block header */
    GET_PRED(bp) = NULL;                  /* pred */
    GET_SUCC(bp) = NULL;                  /* succ */
    PUT(FTRP(bp), PACK(size, 0));         /* Free block footer */
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1)); /* New epilogue header */

    /* Coalesce if the previous block was free */
    return coalesce(bp);
}

/*
 * coalesce - Boundary tag coalescing. Return ptr to coalesced block
 */
static void *coalesce(void *bp)
{
    size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    size_t size = GET_SIZE(HDRP(bp));
    /* Case 1 */
    if (prev_alloc && next_alloc)
    {
        insert_block(bp, size); // insert the block into the free list
        return bp;
    } /* Case 2 */
    else if (prev_alloc && !next_alloc)
    {
        delete_block(NEXT_BLKP(bp));           // delete the next block from the free list
        size += GET_SIZE(HDRP(NEXT_BLKP(bp))); // merge the size
        PUT(HDRP(bp), PACK(size, 0));          // set the header
        PUT(FTRP(bp), PACK(size, 0));          // set the footer
    }                                          /* Case 3 */
    else if (!prev_alloc && next_alloc)
    {
        delete_block(PREV_BLKP(bp));             // delete the previous block from the free list
        size += GET_SIZE(HDRP(PREV_BLKP(bp)));   // merge the size
        PUT(FTRP(bp), PACK(size, 0));            // set the footer
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0)); // set the header
        bp = PREV_BLKP(bp);
    } /* Case 4 */
    else
    {
        delete_block(PREV_BLKP(bp));                                           // delete the previous block from the free list
        delete_block(NEXT_BLKP(bp));                                           // delete the next block from the free list
        size += GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(FTRP(NEXT_BLKP(bp))); // merge the size
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));                               // set the header
        PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));                               // set the footer
        bp = PREV_BLKP(bp);
    }
    GET_PRED(bp) = NULL;    // set the pred to NULL
    GET_SUCC(bp) = NULL;    // set the succ to NULL
    insert_block(bp, size); // insert the block into the free list
    return bp;
}

/*
 * place - Place block of asize bytes at start of free block bp
 *         and split if remainder would be at least minimum block size
 */
static void *place(void *bp, size_t asize)
{
    size_t csize = GET_SIZE(HDRP(bp));
    size_t rsize = csize - asize;

    delete_block(bp);         // delete the block from the free list
    if (rsize <= (2 * DSIZE)) // split the block
    {
        PUT(HDRP(bp), PACK(csize, 1));
        PUT(FTRP(bp), PACK(csize, 1));
    }
    else if (asize >= 96)
    {
        PUT(HDRP(bp), PACK(rsize, 0));
        GET_PRED(bp) = NULL;
        GET_SUCC(bp) = NULL;
        PUT(FTRP(bp), PACK(rsize, 0));
        PUT(HDRP(NEXT_BLKP(bp)), PACK(asize, 1));
        PUT(FTRP(NEXT_BLKP(bp)), PACK(asize, 1));
        coalesce(bp);
        return NEXT_BLKP(bp);
    }
    else
    {
        PUT(HDRP(bp), PACK(asize, 1));
        PUT(FTRP(bp), PACK(asize, 1));
        PUT(HDRP(NEXT_BLKP(bp)), PACK(rsize, 0));
        GET_PRED(NEXT_BLKP(bp)) = NULL;
        GET_SUCC(NEXT_BLKP(bp)) = NULL;
        PUT(FTRP(NEXT_BLKP(bp)), PACK(rsize, 0));
        coalesce(NEXT_BLKP(bp));
    }
    return bp;
}

/*
 * insert_block - Insert a block into the free list
 */
static void insert_block(void *bp, size_t size)
{
    int list_entry_num = 0; /** Keep the track the free list entry number */
    void *insert_ptr = bp;  /* The new block will be inserted after this pointer */
    void *next_ptr = NULL;  /** Keep track of the next pointer the insert_prt */

    /** Select the suitable free list */
    while ((list_entry_num < (LISTSIZE - 1)) && (size > 1))
    {
        size = size >> 1;
        list_entry_num++;
    }

    insert_ptr = LIST_HEAD(list_entry_num);
    if (insert_ptr != NULL)
        next_ptr = GET_SUCC(insert_ptr);
    else
        next_ptr = NULL;
    /* We keep the size order for each free list from the smaller size to the bigger size */
    while ((insert_ptr != NULL) && (GET_SIZE(HDRP(insert_ptr)) < size))
    {
        insert_ptr = next_ptr;
        next_ptr = GET_SUCC(next_ptr);
    }

    if (insert_ptr != NULL)
    {
        /**
         * Insert the new block at the middle of the current free list:
         *      seg_free_listp[list_entry_num] -> ... -> insert_ptr -> bp -> next_ptr -> ... -> NULL
         */
        if (next_ptr != NULL)
        {
            GET_PRED(bp) = insert_ptr;
            GET_SUCC(insert_ptr) = bp;
            GET_SUCC(bp) = next_ptr;
            GET_PRED(next_ptr) = bp;
        }
        /**
         * Insert the new block at the end of the current free list:
         *       seg_free_listp[list_entry_num] -> ... -> insert_ptr -> bp -> NULL(next_ptr)
         */
        else
        {
            GET_PRED(bp) = insert_ptr;
            GET_SUCC(insert_ptr) = bp;
            GET_SUCC(bp) = NULL;
        }
    }
    else
    {
        /**
         * Insert the new block at beginning of the free list;
         * And also there still have other free blocks:
         *      seg_free_listp[list_entry_num](insert_ptr(NULL)) -> bp -> next_ptr -> ... -> NULL
         */
        if (next_ptr != NULL)
        {
            GET_PRED(bp) = NULL;
            GET_SUCC(bp) = next_ptr;
            GET_PRED(next_ptr) = bp;
            LIST_HEAD(list_entry_num) = bp;
        }
        /**
         * The current free list is empty:
         *      seg_free_listp[list_entry_num](insert_ptr(NULL)) -> bp -> NULL(next_ptr)
         */
        else
        {
            GET_PRED(bp) = NULL;
            GET_SUCC(bp) = NULL;
            LIST_HEAD(list_entry_num) = bp;
        }
    }
}

/*
 * delete_block - Delete a block from the free list
 */
static void delete_block(void *bp)
{
    /* Get the size of the block */
    size_t size = GET_SIZE(HDRP(bp));
    /* Get the index of the seg list */
    int index = 0;
    while ((index < LISTSIZE - 1) && (size > 1))
    {
        size = size >> 1;
        index++;
    }

    /* Get the pred and succ of the block */
    void *pred = GET_PRED(bp);
    void *succ = GET_SUCC(bp);
    /* Case 1: */
    if (pred == NULL && succ == NULL)
    {
        LIST_HEAD(index) = NULL; // set the head to NULL
        return;
    }
    /* Case 2: */
    if (pred != NULL && succ == NULL)
    {
        GET_SUCC(GET_PRED(bp)) = NULL; // set the succ of the pred to NULL
        return;
    }
    /* Case 3: */
    if (pred == NULL && succ != NULL)
    {
        LIST_HEAD(index) = GET_SUCC(bp); // set the head to the succ
        GET_PRED(GET_SUCC(bp)) = NULL;   // set the pred of the succ to NULL
        return;
    }
    /* Case 4: */
    GET_SUCC(GET_PRED(bp)) = GET_SUCC(bp); // set the succ of the pred to the succ
    GET_PRED(GET_SUCC(bp)) = GET_PRED(bp); // set the pred of the succ to the pred
}

/*
 * mm_check - Check the heap for consistency
 */
int mm_check(void)
{
    char *bp = heap_listp;
    int index = 0;
    size_t size = 0;
    size_t search = 0;
    while (index < LISTSIZE)
    {
        if (LIST_HEAD(index) != NULL)
        {
            bp = LIST_HEAD(index);
            while (bp != NULL)
            {
                size = GET_SIZE(HDRP(bp));
                search = size;
                while (search > 1)
                {
                    search = search >> 1;
                }
                if (search != (1 << index))
                {
                    printf("Error: The size of the block is not in the correct list\n");
                    return 0;
                }
                bp = GET_SUCC(bp);
            }
        }
        index++;
    }
    return 1;
}

/*
 * mm_checkheap - Check the heap for consistency
 */
void mm_checkheap(int verbose)
{
    char *bp = heap_listp;

    if (verbose)
    {
        printf("Heap (%p):\n", heap_listp);
    }

    if ((GET_SIZE(HDRP(heap_listp)) != DSIZE) || !GET_ALLOC(HDRP(heap_listp)))
    {
        printf("Bad prologue header\n");
    }
    checkblock(heap_listp);

    for (bp = heap_listp; GET_SIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp))
    {
        if (verbose)
        {
            printblock(bp);
        }
        checkblock(bp);
    }

    if (verbose)
    {
        printblock(bp);
    }

    if ((GET_SIZE(HDRP(bp)) != 0) || !(GET_ALLOC(HDRP(bp))))
    {
        printf("Bad epilogue header\n");
    }
}

/*
 * checkblock - Check the block for consistency
 */
static void checkblock(void *bp)
{
    if ((size_t)bp % 8)
    {
        printf("Error: %p is not doubleword aligned\n", bp);
    }
    if (GET(HDRP(bp)) != GET(FTRP(bp)))
    {
        printf("Error: header does not match footer\n");
    }
}

/*
 * printblock - Print the block
 */
static void printblock(void *bp)
{
    size_t hsize, halloc, fsize, falloc;

    hsize = GET_SIZE(HDRP(bp));
    halloc = GET_ALLOC(HDRP(bp));
    fsize = GET_SIZE(FTRP(bp));
    falloc = GET_ALLOC(FTRP(bp));

    if (hsize == 0)
    {
        printf("%p: EOL\n", bp);
        return;
    }

    printf("%p: header: [%lu:%c] footer: [%lu:%c]\n", bp, hsize,
           (halloc ? 'a' : 'f'), fsize, (falloc ? 'a' : 'f'));
}

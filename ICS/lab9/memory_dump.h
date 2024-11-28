#include <stdint.h>
#include <stdio.h>
#include <string.h>
#define BYTES_OF_LINE 8 // 设置每行显示字节长度, 若要跟踪某字节推荐值为8
#define MAX_BYTES_LEN 1600 // 设置最大打印字节数
#define NONE "\033[m"
#define YELLOW "\033[1;33m"
#define GREEN "\033[0;32;32m"
#define LIGHT_RED "\033[1;31m"
#define LIGHT_GREEN "\033[1;32m"
#define LIGHT_CYAN "\033[1;36m"
#define LIGHT_PURPLE "\033[1;35m"

int turn_on_debug = 1;
void *begain_address_debug = (void *) 0x7ffff67ff010;
int bytes_len_debug = 1200;

static int first_debug = 1;
static void *last_begain_address_debug = 0;
static int last_bytes_len_debug = 0;
static unsigned int last_bytes[MAX_BYTES_LEN] = {0};

static void print_short_address(uint64_t idx, void *highlight_ptr1, void *highlight_ptr2)
{
    if(idx == (uint64_t)highlight_ptr1)
    {
        printf(LIGHT_CYAN "\n0x%02x:\t" NONE, (uint16_t)(idx&0xffff));
    }
    else if(idx == (uint64_t)highlight_ptr2)
    {
        printf(LIGHT_PURPLE "\n0x%02x:\t" NONE, (uint16_t)(idx&0xffff));
    }
    else
    {
        printf(LIGHT_GREEN "\n0x%02x:\t" NONE, (uint16_t)(idx&0xffff));
    }
}

void memory_dump(void *ptr, int len, void *highlight_ptr1, void *highlight_ptr2)
{
    // 仅在第一次调用时打印提示信息
    if (first_debug)
    {
        printf("Welcome to debug mode!\nYou can dump memory by setting " YELLOW "begain_address_debug" NONE
               " and " YELLOW
               "bytes_len_debug" NONE ".\n");
        first_debug = 0;
    }
    
    // 只有在打开debug, 指定起始地址, 指定字节数的前提下才会打印
    if (ptr && len && turn_on_debug)
    {
        // 参数与上一次调用是否发生变化
        int is_para_not_changed = 1;
        if (last_begain_address_debug != begain_address_debug || last_bytes_len_debug != bytes_len_debug)
        {
            is_para_not_changed = 0;
            // 发生变化则将缓冲区清空
            memset(last_bytes, 0, sizeof(last_bytes));
        }

        // 打印地址范围
        printf(GREEN "%p ~ %p" NONE, ptr, (uint8_t *)ptr + len - 1);
        // 打印第一个简短地址编号
        uint64_t idx = (uint64_t)ptr;
        print_short_address(idx, highlight_ptr1, highlight_ptr2);
        // 打印每一行
        for (int i = 0; i < len; i++)
        {
            // 在行中间多打印一个空格
            if (i % (BYTES_OF_LINE/2) == 0 && i != 0)
                printf(" ");
            // 打印简短地址编号
            if (i % BYTES_OF_LINE == 0 && i != 0)
            {
                idx += (uint64_t)BYTES_OF_LINE;
                print_short_address(idx, highlight_ptr1, highlight_ptr2);
            }
            // 内存中取值
            uint8_t this_byte = *((uint8_t *)ptr + i);
            // 打印字节
            if (is_para_not_changed && last_bytes[i]!=this_byte)
            {
                printf(LIGHT_RED "%02x " NONE, this_byte);
            }
            else
            {
                printf("%02x ", this_byte);
            }
            // 更新字节
            last_bytes[i] = this_byte;
        }
        printf("\n");
        // 置参
        last_begain_address_debug = begain_address_debug;
        last_bytes_len_debug = bytes_len_debug;
    }
}

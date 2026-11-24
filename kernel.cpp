#include <stdint.h>

// надёжная запись байта в порт: %b0 = младший байт, %w1 = 16-битный порт
static inline void outb(uint16_t port, uint8_t val) {
    asm volatile("outb %b0, %w1" : : "a"(val), "d"(port));
}

namespace serial {
    void init() {
        outb(0x3F8 + 1, 0);   // прерывания off
        outb(0x3F8 + 3, 0x03); // 8 бит, 1 стоп-бит
    }

    void putc(char c) {
        outb(0x3F8, (uint8_t)c);
    }

    void puts(const char* s) {
        while (*s) putc(*s++);
    }
}

namespace vga {
    constexpr int W = 80;
    constexpr int H = 25;
    volatile uint16_t* const buffer = (volatile uint16_t*)0xB8000;

    enum color : uint8_t {
        black = 0, blue = 1, green = 2, cyan = 3,
        red = 4, magenta = 5, brown = 6, light_gray = 7
    };

    inline uint16_t entry(char c, uint8_t fg, uint8_t bg) {
        return (uint16_t)c | (uint16_t)(fg | (bg << 4)) << 8;
    }

    static int cur_x = 0;
    static int cur_y = 0;

    void clear() {
        for (int i = 0; i < W * H; ++i)
            buffer[i] = entry(' ', light_gray, black);
        cur_x = cur_y = 0;
    }

    void scroll() {
        for (int i = 0; i < W * (H - 1); ++i)
            buffer[i] = buffer[i + W];
        for (int i = W * (H - 1); i < W * H; ++i)
            buffer[i] = entry(' ', light_gray, black);
        cur_y = H - 1;
    }

    void putc(char c) {
        if (c == '\n') { cur_x = 0; ++cur_y; }
        else {
            buffer[cur_y * W + cur_x] = entry(c, light_gray, black);
            if (++cur_x >= W) { cur_x = 0; ++cur_y; }
        }
        if (cur_y >= H) scroll();
    }

    void puts(const char* s) {
        while (*s) putc(*s++);
    }
}

extern "C" void kernel_main() {
    serial::init();
    serial::puts("[serial] kernel started\n");

    vga::clear();
    vga::puts("=== My OS ===\n\n");
    vga::puts("Console output works!\n");

    for (int i = 1; i <= 30; ++i) {
        vga::puts("line ");
        char buf[16]; int n = i, pos = 0;
        char tmp[16]; int t = 0;
        while (n > 0) { tmp[t++] = '0' + n % 10; n /= 10; }
        while (t > 0) buf[pos++] = tmp[--t];
        buf[pos] = 0;
        vga::puts(buf);
        vga::puts("\n");
    }

    serial::puts("[serial] done\n");
}


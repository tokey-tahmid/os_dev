#include <config.h>
#include <compiler.h>
#include <log.h>
#include <stdarg.h>
#include <csr.h>

static int k_log_level = 0xFFF;

static int vlogf(log_type lt, const char *fmt, va_list args)
{
    if (!(lt & k_log_level)) {
        return 0;
    }
    int vprintf_(const char *format, va_list va);
    return vprintf_(fmt, args);
}

int logf(log_type lt, const char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    int ret = vlogf(lt, fmt, va);
    va_end(va);

    return ret;
}

void klog(log_type lt, bool on)
{
    if (on) {
        k_log_level |= lt;
    }
    else {
        k_log_level &= ~lt;
    }
}

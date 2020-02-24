#ifndef PNGQUANT_H
#define PNGQUANT_H

#include "rwpng.h"  /* typedefs, common macros, public prototypes */
#include "lib/libimagequant.h"

struct pngquant_options {
    liq_attr *liq;
    liq_image *fixed_palette_image;
    liq_log_callback_function *log_callback;
    void *log_callback_user_info;
    float floyd;
    bool using_stdin, using_stdout, force, fast_compression, ie_mode,
        min_quality_limit, skip_if_larger,
        verbose;
};

pngquant_error pngquant_file(const char *filename, const char *outname, struct pngquant_options *options);

#endif // PNGQUANT_H

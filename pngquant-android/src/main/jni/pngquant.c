/* pngquant.c - quantize the colors in an alphamap down to a specified number
**
** Copyright (C) 1989, 1991 by Jef Poskanzer.
**
** Permission to use, copy, modify, and distribute this software and its
** documentation for any purpose and without fee is hereby granted, provided
** that the above copyright notice appear in all copies and that both that
** copyright notice and this permission notice appear in supporting
** documentation.  This software is provided "as is" without express or
** implied warranty.
**
** - - - -
**
** © 1997-2002 by Greg Roelofs; based on an idea by Stefan Schneider.
** © 2009-2015 by Kornel Lesiński.
**
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification,
** are permitted provided that the following conditions are met:
**
** 1. Redistributions of source code must retain the above copyright notice,
**    this list of conditions and the following disclaimer.
**
** 2. Redistributions in binary form must reproduce the above copyright notice,
**    this list of conditions and the following disclaimer in the documentation
**    and/or other materials provided with the distribution.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
** FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
** DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
** SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
** CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
** OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include <getopt.h>
#include <unistd.h>

#include "pngquant.h"

extern char *optarg;
extern int optind, opterr;

#if defined(WIN32) || defined(__WIN32__)
#  include <fcntl.h>    /* O_BINARY */
#  include <io.h>   /* setmode() */
#endif

#ifdef _OPENMP
#include <omp.h>
#else
#define omp_get_max_threads() 1
#define omp_get_thread_num() 0
#endif


static pngquant_error prepare_output_image(liq_result *result, liq_image *input_image, png8_image *output_image);
static void set_palette(liq_result *result, png8_image *output_image);
static pngquant_error read_image(liq_attr *options, const char *filename, int using_stdin, png24_image *input_image_p, liq_image **liq_image_p, bool keep_input_pixels, bool verbose);
static pngquant_error write_image(png8_image *output_image, png24_image *output_image24, const char *outname, struct pngquant_options *options);
static char *add_filename_extension(const char *filename, const char *newext);
static bool file_exists(const char *outname);

static void verbose_printf(struct pngquant_options *context, const char *fmt, ...)
{
    if (context->log_callback) {
        va_list va;
        va_start(va, fmt);
        int required_space = vsnprintf(NULL, 0, fmt, va)+1; // +\0
        va_end(va);

        char buf[required_space];
        va_start(va, fmt);
        vsnprintf(buf, required_space, fmt, va);
        va_end(va);

        context->log_callback(context->liq, buf, context->log_callback_user_info);
    }
}

static void log_callback(const liq_attr *attr, const char *msg, void* user_info)
{
    fprintf(stderr, "%s\n", msg);
}

#ifdef _OPENMP
#define LOG_BUFFER_SIZE 1300
struct buffered_log {
    int buf_used;
    char buf[LOG_BUFFER_SIZE];
};

static void log_callback_buferred_flush(const liq_attr *attr, void *context)
{
    struct buffered_log *log = context;
    if (log->buf_used) {
        fwrite(log->buf, 1, log->buf_used, stderr);
        fflush(stderr);
        log->buf_used = 0;
    }
}

static void log_callback_buferred(const liq_attr *attr, const char *msg, void* context)
{
    struct buffered_log *log = context;
    int len = strlen(msg);
    if (len > LOG_BUFFER_SIZE-2) len = LOG_BUFFER_SIZE-2;

    if (len > LOG_BUFFER_SIZE - log->buf_used - 2) log_callback_buferred_flush(attr, log);
    memcpy(&log->buf[log->buf_used], msg, len);
    log->buf_used += len+1;
    log->buf[log->buf_used-1] = '\n';
    log->buf[log->buf_used] = '\0';
}
#endif

enum {arg_floyd=1, arg_ordered, arg_ext, arg_no_force, arg_iebug,
    arg_transbug, arg_map, arg_posterize, arg_skip_larger};

static const struct option long_options[] = {
    {"verbose", no_argument, NULL, 'v'},
    {"quiet", no_argument, NULL, 'q'},
    {"force", no_argument, NULL, 'f'},
    {"no-force", no_argument, NULL, arg_no_force},
    {"floyd", optional_argument, NULL, arg_floyd},
    {"ordered", no_argument, NULL, arg_ordered},
    {"nofs", no_argument, NULL, arg_ordered},
    {"iebug", no_argument, NULL, arg_iebug},
    {"transbug", no_argument, NULL, arg_transbug},
    {"ext", required_argument, NULL, arg_ext},
    {"skip-if-larger", no_argument, NULL, arg_skip_larger},
    {"output", required_argument, NULL, 'o'},
    {"speed", required_argument, NULL, 's'},
    {"quality", required_argument, NULL, 'Q'},
    {"posterize", required_argument, NULL, arg_posterize},
    {"map", required_argument, NULL, arg_map},
    {"version", no_argument, NULL, 'V'},
    {"help", no_argument, NULL, 'h'},
    {NULL, 0, NULL, 0},
};


pngquant_error pngquant_file(const char *filename, const char *outname, struct pngquant_options *options)
{
    pngquant_error retval = SUCCESS;

    verbose_printf(options, "%s:", filename);

    liq_image *input_image = NULL;
    png24_image input_image_rwpng = {};
    bool keep_input_pixels = options->skip_if_larger || (options->using_stdout && options->min_quality_limit); // original may need to be output to stdout
    if (SUCCESS == retval) {
        retval = read_image(options->liq, filename, options->using_stdin, &input_image_rwpng, &input_image, keep_input_pixels, options->verbose);
    }

    int quality_percent = 90; // quality on 0-100 scale, updated upon successful remap
    png8_image output_image = {};
    if (SUCCESS == retval) {
        verbose_printf(options, "  read %luKB file", (input_image_rwpng.file_size+1023UL)/1024UL);

#if USE_LCMS
        if (input_image_rwpng.lcms_status == ICCP) {
            verbose_printf(options, "  used embedded ICC profile to transform image to sRGB colorspace");
        } else if (input_image_rwpng.lcms_status == GAMA_CHRM) {
            verbose_printf(options, "  used gAMA and cHRM chunks to transform image to sRGB colorspace");
        } else if (input_image_rwpng.lcms_status == ICCP_WARN_GRAY) {
            verbose_printf(options, "  warning: ignored ICC profile in GRAY colorspace");
        }
#endif

        if (input_image_rwpng.gamma != 0.45455) {
            verbose_printf(options, "  corrected image from gamma %2.1f to sRGB gamma",
                           1.0/input_image_rwpng.gamma);
        }

        // when using image as source of a fixed palette the palette is extracted using regular quantization
        liq_result *remap = liq_quantize_image(options->liq, options->fixed_palette_image ? options->fixed_palette_image : input_image);

        if (remap) {
            liq_set_output_gamma(remap, 0.45455); // fixed gamma ~2.2 for the web. PNG can't store exact 1/2.2
            liq_set_dithering_level(remap, options->floyd);

            retval = prepare_output_image(remap, input_image, &output_image);
            if (SUCCESS == retval) {
                if (LIQ_OK != liq_write_remapped_image_rows(remap, input_image, output_image.row_pointers)) {
                    retval = OUT_OF_MEMORY_ERROR;
                }

                set_palette(remap, &output_image);

                double palette_error = liq_get_quantization_error(remap);
                if (palette_error >= 0) {
                    quality_percent = liq_get_quantization_quality(remap);
                    verbose_printf(options, "  mapped image to new colors...MSE=%.3f (Q=%d)", palette_error, quality_percent);
                }
            }
            liq_result_destroy(remap);
        } else {
            retval = TOO_LOW_QUALITY;
        }
    }

    if (SUCCESS == retval) {

        if (options->skip_if_larger) {
            // this is very rough approximation, but generally avoid losing more quality than is gained in file size.
            // Quality is squared, because even greater savings are needed to justify big quality loss.
            double quality = quality_percent/100.0;
            output_image.maximum_file_size = (input_image_rwpng.file_size-1) * quality*quality;
        }

        output_image.fast_compression = options->fast_compression;
        output_image.chunks = input_image_rwpng.chunks; input_image_rwpng.chunks = NULL;
        retval = write_image(&output_image, NULL, outname, options);

        if (TOO_LARGE_FILE == retval) {
            verbose_printf(options, "  file exceeded expected size of %luKB", (unsigned long)output_image.maximum_file_size/1024UL);
        }
    }

    if (options->using_stdout && keep_input_pixels && (TOO_LARGE_FILE == retval || TOO_LOW_QUALITY == retval)) {
        // when outputting to stdout it'd be nasty to create 0-byte file
        // so if quality is too low, output 24-bit original
        pngquant_error write_retval = write_image(NULL, &input_image_rwpng, outname, options);
        if (write_retval) {
            retval = write_retval;
        }
    }

    if (input_image) liq_image_destroy(input_image);
    rwpng_free_image24(&input_image_rwpng);
    rwpng_free_image8(&output_image);

    return retval;
}

static void set_palette(liq_result *result, png8_image *output_image)
{
    const liq_palette *palette = liq_get_palette(result);

    // tRNS, etc.
    output_image->num_palette = palette->count;
    output_image->num_trans = 0;
    for(unsigned int i=0; i < palette->count; i++) {
        liq_color px = palette->entries[i];
        if (px.a < 255) {
            output_image->num_trans = i+1;
        }
        output_image->palette[i] = (png_color){.red=px.r, .green=px.g, .blue=px.b};
        output_image->trans[i] = px.a;
    }
}


static bool file_exists(const char *outname)
{
    FILE *outfile = fopen(outname, "rb");
    if ((outfile ) != NULL) {
        fclose(outfile);
        return true;
    }
    return false;
}

/* build the output filename from the input name by inserting "-fs8" or
 * "-or8" before the ".png" extension (or by appending that plus ".png" if
 * there isn't any extension), then make sure it doesn't exist already */
static char *add_filename_extension(const char *filename, const char *newext)
{
    size_t x = strlen(filename);

    char* outname = malloc(x+4+strlen(newext)+1);
    if (!outname) return NULL;

    strncpy(outname, filename, x);
    if (strncmp(outname+x-4, ".png", 4) == 0 || strncmp(outname+x-4, ".PNG", 4) == 0) {
        strcpy(outname+x-4, newext);
    } else {
        strcpy(outname+x, newext);
    }

    return outname;
}

static char *temp_filename(const char *basename) {
    size_t x = strlen(basename);

    char *outname = malloc(x+1+4);
    if (!outname) return NULL;

    strcpy(outname, basename);
    strcpy(outname+x, ".tmp");

    return outname;
}

static void set_binary_mode(FILE *fp)
{
#if defined(WIN32) || defined(__WIN32__)
    setmode(fp == stdout ? 1 : 0, O_BINARY);
#endif
}

static const char *filename_part(const char *path)
{
    const char *outfilename = strrchr(path, '/');
    if (outfilename) {
        return outfilename+1;
    } else {
        return path;
    }
}

static bool replace_file(const char *from, const char *to, const bool force) {
#if defined(WIN32) || defined(__WIN32__)
    if (force) {
        // On Windows rename doesn't replace
        unlink(to);
    }
#endif
    return (0 == rename(from, to));
}

static pngquant_error write_image(png8_image *output_image, png24_image *output_image24, const char *outname, struct pngquant_options *options)
{
    FILE *outfile;
    char *tempname = NULL;

    if (options->using_stdout) {
        set_binary_mode(stdout);
        outfile = stdout;

        if (output_image) {
            verbose_printf(options, "  writing %d-color image to stdout", output_image->num_palette);
        } else {
            verbose_printf(options, "  writing truecolor image to stdout");
        }
    } else {
        tempname = temp_filename(outname);
        if (!tempname) return OUT_OF_MEMORY_ERROR;

        if ((outfile = fopen(tempname, "wb")) == NULL) {
            fprintf(stderr, "  error: cannot open '%s' for writing\n", tempname);
            free(tempname);
            return CANT_WRITE_ERROR;
        }

        if (output_image) {
            verbose_printf(options, "  writing %d-color image as %s", output_image->num_palette, filename_part(outname));
        } else {
            verbose_printf(options, "  writing truecolor image as %s", filename_part(outname));
        }
    }

    pngquant_error retval;
    #pragma omp critical (libpng)
    {
        if (output_image) {
            retval = rwpng_write_image8(outfile, output_image);
        } else {
            retval = rwpng_write_image24(outfile, output_image24);
        }
    }

    if (!options->using_stdout) {
        fclose(outfile);

        if (SUCCESS == retval) {
            // Image has been written to a temporary file and then moved over destination.
            // This makes replacement atomic and avoids damaging destination file on write error.
            if (!replace_file(tempname, outname, options->force)) {
                retval = CANT_WRITE_ERROR;
            }
        }

        if (retval) {
            unlink(tempname);
        }
    }
    free(tempname);

    if (retval && retval != TOO_LARGE_FILE) {
        fprintf(stderr, "  error: failed writing image to %s\n", outname);
    }

    return retval;
}

static pngquant_error read_image(liq_attr *options, const char *filename, int using_stdin, png24_image *input_image_p, liq_image **liq_image_p, bool keep_input_pixels, bool verbose)
{
    FILE *infile;

    if (using_stdin) {
        set_binary_mode(stdin);
        infile = stdin;
    } else if ((infile = fopen(filename, "rb")) == NULL) {
        fprintf(stderr, "  error: cannot open %s for reading\n", filename);
        return READ_ERROR;
    }

    pngquant_error retval;
    #pragma omp critical (libpng)
    {
        retval = rwpng_read_image24(infile, input_image_p, verbose);
    }

    if (!using_stdin) {
        fclose(infile);
    }

    if (retval) {
        fprintf(stderr, "  error: cannot decode image %s\n", using_stdin ? "from stdin" : filename_part(filename));
        return retval;
    }

    *liq_image_p = liq_image_create_rgba_rows(options, (void**)input_image_p->row_pointers, input_image_p->width, input_image_p->height, input_image_p->gamma);

    if (!*liq_image_p) {
        return OUT_OF_MEMORY_ERROR;
    }

    if (!keep_input_pixels) {
        if (LIQ_OK != liq_image_set_memory_ownership(*liq_image_p, LIQ_OWN_ROWS | LIQ_OWN_PIXELS)) {
            return OUT_OF_MEMORY_ERROR;
        }
        input_image_p->row_pointers = NULL;
        input_image_p->rgba_data = NULL;
    }

    return SUCCESS;
}

static pngquant_error prepare_output_image(liq_result *result, liq_image *input_image, png8_image *output_image)
{
    output_image->width = liq_image_get_width(input_image);
    output_image->height = liq_image_get_height(input_image);
    output_image->gamma = liq_get_output_gamma(result);

    /*
    ** Step 3.7 [GRR]: allocate memory for the entire indexed image
    */

    output_image->indexed_data = malloc(output_image->height * output_image->width);
    output_image->row_pointers = malloc(output_image->height * sizeof(output_image->row_pointers[0]));

    if (!output_image->indexed_data || !output_image->row_pointers) {
        return OUT_OF_MEMORY_ERROR;
    }

    for(size_t row = 0; row < output_image->height; row++) {
        output_image->row_pointers[row] = output_image->indexed_data + row * output_image->width;
    }

    const liq_palette *palette = liq_get_palette(result);
    // tRNS, etc.
    output_image->num_palette = palette->count;
    output_image->num_trans = 0;
    for(unsigned int i=0; i < palette->count; i++) {
        if (palette->entries[i].a < 255) {
            output_image->num_trans = i+1;
        }
    }

    return SUCCESS;
}

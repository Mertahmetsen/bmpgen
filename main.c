#include <stdio.h>
#include <cargs.h>
#include <raylib.h>
#include <stdlib.h>

typedef Color Pixel;

const char* scriptFile = "default.script";
const char* outputFile = "default.bin";
const char* inputFile = "default.bin";
int imgl = 640, imgh = 480;
static struct cag_option options[] = {
    {
        .identifier = 's',
        .access_letters = "sS",
        .access_name = "script",
        .value_name = "PATH",
        .description = "Execute script file"
    },
    {
        .identifier = 'h',
        .access_letters = "h",
        .access_name = "help",
        .description = "Show this help message"
        },{
            .identifier = 'o',
            .access_letters = "oO",
            .access_name = "out",
            .value_name = "PATH",
            .description = "The output file for script execution"
        }, {
            .identifier = 'v',
            .access_letters = "vVwW",
            .access_name = "view-img",
            .value_name = "PATH",
            .description = "Views raw bitmap image"},
    {
        .identifier = 'l',
        .access_letters = "lL",
        .access_name = "length",
        .value_name = "VALUE",
        .description = "Length for viewing bitmap image"},
    {
        .identifier = 'e',
        .access_letters = "H",
        .access_name = "height",
        .value_name = "VALUE",
        .description = "Height for viewing bitmap image"},
    {
        .identifier = 'e',
        .access_letters = "H",
        .access_name = "height",
        .value_name = "VALUE",
        .description = "Height for viewing bitmap image"}
};

long getFileSize (FILE* f) {
    if (fseek(f, 0, SEEK_END) != 0) { perror("fseek"); fclose(f); exit(1); }
    long size = ftell(f);
    if (size < 0) { perror("ftell"); fclose(f); exit(1); }
    rewind(f);
    return size;
}

void printHelpMsg (void) {
    puts("Usage: bmpgen [options]");
    puts("32-bit bitmap generator, compressor/decompressor and 32-bit bitmap image viewer.");
    puts("bmpgen can interpret uncompressed 32-bit bitmaps as images and view them.");
    cag_option_print(options, CAG_ARRAY_SIZE(options), stdout);
}

void script (void) {
    Pixel testdataout[] = {VIOLET, BLUE, GREEN, YELLOW, WHITE, BLACK}, *testdatain;
    FILE* f = fopen("test.bin", "wb");
    if (!f) {
        perror ("fopen");
    }
    if (fwrite(testdataout, sizeof(testdataout), 1, f) != 1) {
        perror ("fwrite");
        fclose (f);
    }
    fclose(f);
    f = fopen("test.bin", "rb");
    if (!f) {
        perror ("fopen");
    }
    if (fread(testdatain, sizeof(testdatain), 1, f) != 1) {
        perror ("fread");
        fclose (f);
    }
    fclose(f);
}

void view(void) {
    FILE *f = fopen(inputFile, "rb");
    if (!f) { perror("fopen"); exit(1); }

    long fsize = getFileSize(f);
    if (fsize < 0) { fprintf(stderr, "Error while seeking filesize\n"); fclose(f); exit(1); }

    size_t count = (size_t)fsize / sizeof(Pixel);
    size_t expected = (size_t)imgl * imgh;
    if ((size_t)fsize % sizeof(Pixel) != 0) {
        fprintf(stderr, "WARNING: Input file is not a 32-bit bitmap. The image may appear corrupt.\n");
    }
    if (count != expected) {
        fprintf(stderr, "WARNING: file has %zu pixels but %dx%d needs %zu\n",
                count, imgl, imgh, expected);
    }

    // calloc zeroes the buffer, so any missing pixels show up as black/transparent
    Pixel *buffer = calloc(expected, sizeof *buffer);
    if (!buffer) { perror("calloc"); fclose(f); exit(1); }

    size_t toRead = count < expected ? count : expected;
    if (fread(buffer, sizeof *buffer, toRead, f) != toRead) {
        perror("fread");
        free(buffer);
        fclose(f);
        exit(1);
    }
    fclose(f);

    InitWindow(imgl, imgh, TextFormat("bmpgen - %s", inputFile));
    SetTargetFPS(60);

    Image img = {
        .data = buffer,
        .width = imgl,
        .height = imgh,
        .mipmaps = 1,
        .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8,
    };
    Texture2D tex = LoadTextureFromImage(img);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        DrawTexture(tex, 0, 0, WHITE);
        EndDrawing();
    }

    UnloadTexture(tex);
    free(buffer);
    CloseWindow();
}

int main(int argc, char *argv[]) {
    system("pwd");
    if (argc < 2) {
        printHelpMsg();
        return 0;
    }
    bool run_script = false, view_img = false;
    cag_option_context context;
    cag_option_init(&context, options, CAG_ARRAY_SIZE(options), argc, argv);
    while (cag_option_fetch(&context)) {
        switch (cag_option_get_identifier(&context)) {
            case 's':
                scriptFile = cag_option_get_value(&context);
                run_script = true;
                break;
            case 'h':
                printHelpMsg();
                break;
            case 'o':
                outputFile =  cag_option_get_value(&context);
                break;
            case 'v':
                inputFile = cag_option_get_value(&context);
                view_img = true;
                break;
            case 'l':
                imgl = TextToInteger(cag_option_get_value(&context));
                break;
            case 'e':
                imgh = TextToInteger(cag_option_get_value(&context));
                break;
            default:
                printHelpMsg();
                break;
        }
    }
    if (run_script) script ();
    if (view_img) view ();

    return 0;
}

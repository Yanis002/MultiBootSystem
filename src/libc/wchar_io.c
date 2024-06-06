#include "stdio.h"

int fwide(FILE* stream, int mode) {
    int orientation;

    if (stream == NULL || stream->file_mode.file_kind == __closed_file) {
        return 0;
    }

    orientation = stream->file_mode.file_orientation;

    switch (orientation) {
        case UNORIENTED:
            if (mode > 0) {
                stream->file_mode.file_orientation = WIDE_ORIENTED;
            } else if (mode < 0) {
                stream->file_mode.file_orientation = CHAR_ORIENTED;
            }

            return mode;

        case WIDE_ORIENTED:
            return 1;

        case CHAR_ORIENTED:
            return -1;
    }
}

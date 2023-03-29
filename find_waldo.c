#include <stdlib.h>
#include <stdio.h>
#include <jpeglib.h>

void print_help();
void readMessage(FILE*, int, j_decompress_ptr, jvirt_barray_ptr*);

int main(int argc, char *argv[]) {

    const int IN_FILE_INDEX = 1;
    const int OUT_FILE_INDEX = 2;

    struct jpeg_decompress_struct dcompStruct;
    struct jpeg_error_mgr jpegErr;
    jvirt_barray_ptr *dctCoeffs;

    FILE *infile;
    FILE *outfile;



    if(argc != 3) {
        printf("\nError: Invalid number of arguments\n");
        print_help();
        return EXIT_FAILURE;
    }

    // Open files
    infile = fopen(argv[IN_FILE_INDEX], "r");
    outfile = fopen(argv[OUT_FILE_INDEX], "w");

    if(infile == NULL || outfile == NULL) {
        printf("\nError: Unable to open file\n");
        print_help();
        return EXIT_FAILURE;
    }


    // Initialize compression and decompression structs
    jpeg_create_decompress(&dcompStruct);

    dcompStruct.err = jpeg_std_error(&jpegErr);

    jpeg_stdio_src(&dcompStruct, infile);



    // Read the input JPEG file's DCT coefficients
    jpeg_read_header(&dcompStruct, TRUE);
    dctCoeffs = jpeg_read_coefficients(&dcompStruct);


    // Read and store the hidden data
    readMessage(outfile, 0, &dcompStruct, dctCoeffs);


    // Finish and free allocated objects
    jpeg_finish_decompress(&dcompStruct);
    jpeg_destroy_decompress(&dcompStruct);

    fclose(infile);
    fclose(outfile);

    return EXIT_SUCCESS;
}


// This help text was written by Chat-GPT Mar 14 Version, 2023
void print_help() {
    printf("Usage: myprogram INPUT_IMAGE OUTPUT_FILE\n\n  Read a hidden message from a JPEG file.\n\nArguments:\n  INPUT_IMAGE     The path to the input JPEG image.\n  OUTPUT_FILE     The path to the output text file, where the hidden text data will be written.\n");
}


void readMessage(FILE* file, int seed, j_decompress_ptr imgInfo, jvirt_barray_ptr *coeffs) {

    unsigned int bit;
    unsigned char byte = 0;
    int bytesWritten;

    int block_size = DCTSIZE * imgInfo->comp_info[0].h_samp_factor * imgInfo->comp_info[0].v_samp_factor;
    int offset = (block_size * block_size) - 8;

    JBLOCKARRAY buffer;
    JCOEFPTR block;
    JDIMENSION col;
    JDIMENSION row;

    // Loop through the bytes of the input file and the blocks of the image
    for(row = 0; row < imgInfo->comp_info[0].height_in_blocks; row++) {

        buffer = (imgInfo->mem->access_virt_barray)((j_common_ptr)imgInfo, coeffs[0], row, (JDIMENSION)1, TRUE);

        for(col = 0; col < imgInfo->comp_info[0].width_in_blocks; col++) {

            block = buffer[0][col];

            if(block[offset] == 2) {
                break;
            }

            byte = 0;
            for(int i = 7; i >= 0; i--) {
                bit = block[offset + (7 - i)];

                byte |= (bit << i);
                
            }

            bytesWritten = fwrite(&byte, 1, 1, file);

            if(bytesWritten != 1) {
                break;
            }
        }

        if(block[offset] == 2 || bytesWritten != 1) {
            break;
        }
    }

    // Add a newline at the end of the file to follow convention
    fwrite("\n", 1, 1, file);
}

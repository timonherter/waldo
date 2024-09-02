#include <stdlib.h>
#include <stdio.h>
#include <jpeglib.h>

void print_help();
void updateCoefficients(FILE*, int, j_decompress_ptr, jvirt_barray_ptr*);

int main(int argc, char *argv[]) {

    const int MSG_FILE_INDEX = 1;
    const int IN_FILE_INDEX = 2;
    const int OUT_FILE_INDEX = 3;

    struct jpeg_decompress_struct dcompStruct;
    struct jpeg_compress_struct compStruct;
    struct jpeg_error_mgr jpegErr;
    jvirt_barray_ptr *dctCoeffs;

    FILE *msgfile;
    FILE *infile;
    FILE *outfile;



    if(argc != 4) {
        printf("\nError: Invalid number of arguments\n");
        print_help();
        return EXIT_FAILURE;
    }

    // Open files
    msgfile = fopen(argv[MSG_FILE_INDEX], "r");
    infile = fopen(argv[IN_FILE_INDEX], "r");
    outfile = fopen(argv[OUT_FILE_INDEX], "w");

    if(msgfile == NULL || infile == NULL || outfile == NULL) {
        printf("\nError: Unable to open file\n");
        print_help();
        return EXIT_FAILURE;
    }


    // Initialize compression and decompression structs
    jpeg_create_compress(&compStruct);
    jpeg_create_decompress(&dcompStruct);

    compStruct.err = jpeg_std_error(&jpegErr);
    dcompStruct.err = jpeg_std_error(&jpegErr);

    jpeg_stdio_dest(&compStruct, outfile);
    jpeg_stdio_src(&dcompStruct, infile);



    // Read the input JPEG file's DCT coefficients
    jpeg_read_header(&dcompStruct, TRUE);
    dctCoeffs = jpeg_read_coefficients(&dcompStruct);

    // Write the message data to the DCT coefficients
    updateCoefficients(msgfile, 0, &dcompStruct, dctCoeffs);

    // Copy the header and edited coefficients to the compression object
    jpeg_copy_critical_parameters(&dcompStruct, &compStruct);
    jpeg_write_coefficients(&compStruct, dctCoeffs);



    // Finish and free allocated objects
    jpeg_finish_compress(&compStruct);
    jpeg_destroy_compress(&compStruct);
    jpeg_finish_decompress(&dcompStruct);
    jpeg_destroy_decompress(&dcompStruct);
    /*
    for(int i = 0; i < dcompStruct.num_components; i++) {
        jpeg_free_large((j_common_ptr) &dcompStruct, dctCoeffs[i]->mem_buffer);
    }

    jpeg_free_large((j_common_ptr) &dcompStruct, dctCoeffs);
    */
    fclose(msgfile);
    fclose(infile);
    fclose(outfile);

    return EXIT_SUCCESS;
}


// This help text was written by Chat-GPT Mar 14 Version, 2023
void print_help() {
    printf("\nUsage: myprogram MESSAGE_FILE INPUT_IMAGE OUTPUT_IMAGE\n\n  Embed a message file into a JPEG image and save the result as a new JPEG image.\n\nArguments:\n  MESSAGE_FILE    The path to the text file containing the message to be embedded.\n  INPUT_IMAGE     The path to the input JPEG image.\n  OUTPUT_IMAGE    The path to the output JPEG image, where the embedded message will be stored.\n");
}


void updateCoefficients(FILE* file, int seed, j_decompress_ptr imgInfo, jvirt_barray_ptr *coeffs) {

    unsigned int bit;
    int next = fgetc(file);

    int block_size = DCTSIZE * imgInfo->comp_info[0].h_samp_factor * imgInfo->comp_info[0].v_samp_factor;
    int offset = (block_size * block_size) - 8;

    JBLOCKARRAY buffer;
    JCOEFPTR block;
    JDIMENSION row;
    JDIMENSION col;

    // Loop through the bytes of the input file and the blocks of the image
    for(row = 0; row < imgInfo->comp_info[0].height_in_blocks && next != EOF; row++) {

        buffer = (imgInfo->mem->access_virt_barray)((j_common_ptr)imgInfo, coeffs[0], row, (JDIMENSION)1, TRUE);

        for(col = 0; col < imgInfo->comp_info[0].width_in_blocks && next != EOF; col++) {

            block = buffer[0][col];

            for(int i = 7; i >= 0; i--) {
                bit = (next >> i) & 1;
                
                block[offset + (7 - i)] = bit;
            }

            next = fgetc(file);
        }

        // If that was the last byte in the input, insert an end of file marker
        if(next == EOF && col < imgInfo->comp_info[0].width_in_blocks) {
            
            block[offset] = 2;
        } else if(next == EOF && row < imgInfo->comp_info[0].height_in_blocks) {
            buffer = (imgInfo->mem->access_virt_barray)((j_common_ptr)imgInfo, coeffs[0], row + 1, (JDIMENSION)1, TRUE);
            block = buffer[0][0];

            block[offset] = 2;
        }
    }
}

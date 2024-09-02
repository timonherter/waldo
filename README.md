# Waldo - Steganography in Compressed JPEG Images
### Timon Herter

## This project is a work in progress. Commits are not backward compatable.
### Remember the liability disclaimer.

## Overview
This project hides messages within the image data of JPEG-1 encoded images.

While encrption keeps a message from being read by an unintended recipient, steganography hides the fact that a message
was sent at all from anybody but the intended recipient.


## Dependencies

This project uses the libjpeg API. I used the [libjpeg-turbo](https://libjpeg-turbo.org/) library which implements this
API. You should be able to use any library that implements the API, but you may need to change the include flag in the
make file and some file headers.


## Building

0. You should have gcc and make installed already
1. Install the libjpeg-turbo API.
2. Run the `make` command


## Usage

This repo contains two programs: hide_waldo.c and find_waldo.c

hide_waldo takes a text file which contains your hidden message, an input jpeg file, and a name or path to an
output jpeg file.

find_waldo takes an input jpeg file which has a message hidden in it. It also takes the name or path to an output text
file where it will write the hidden message.


## Roadmap

+ Development: Modify pseudorandom blocks in the image rather than sequential blocks
+ Development: Add functionality to insert arbitrary files rather than just text
+ Development: Add functionality to encrypt and decrypt data as it is inserted or read from the image

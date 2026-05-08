# cat-reader
<img width="720" height="720" alt="image" src="https://github.com/MorceNOX/cat-reader/blob/main/assets/image_.png" />

 Choose your e-book, article or select any text and the reader will speak it out loud, sentence by sentence, while you read it in big ASCII Art letters!
 
 Navigate as you wish, and control everything easily: the speed, the language, the volume of the audio and even the colors!
 
 This is, by far, the most comfortable way to read a book and to be engaged in your favourite readings!

## Requirements
 
- gcc
- bash
- make
- pandoc
- poppler-utils
- jq
- newt

 You need a `gcc compiler`, `make` and `bash` to run the `Makefile` and generate the executable when you are building from source, which is the preferable method. It is simple and it guarantees that everything will work without issues on your personal machine and environment.
 `Bash` is also required to run the initial menu and configuration options.
 `Pandoc` is required for reading `.markdown` files, `.epub` files, `.docx` files and `.rtf` files.
 `Poppler-utils` is required for converting `.pdf` files into text, so you can read your .pdf files which contains text as if they were a simple plain text file.
 `jq` and `newt` is normally already installed on most ditros, but if not, you have to install these packages do make the application able to read configuration files and to display the menus properly.
### Optional
- ascii-image-converter
 Just choose the version that fits your machine, extract the files and copy the binary to `/usr/local/bin`.
 Here is the link: [ascii-image-converter last release](https://github.com/TheZoraiz/ascii-image-converter/releases/tag/v1.13.1).
 This program is needed to show the image in *ASCII Art* format in the splash screen and when the application finnishes. It gives a little more *charm* to the reader, showing our little black cat reading a book, but if not installed, the Reader will still work normally.

# cat-reader
<img width="720" height="720" alt="image" src="https://github.com/MorceNOX/cat-reader/blob/main/assets/image_.png" />

 Choose your e-book, article or select any text and the reader will speak it out loud, sentence by sentence, while you read it in big ASCII Art letters!
 
 Navigate as you wish, and control everything easily: the speed, the language, the volume of the audio and even the colors!
 
 This is, by far, the most comfortable way to read a book and to be engaged in your favourite readings!

 `cat-reader` stands for C Art Text Reader, because is written in `C Language` and provides a way to read your `Text` in an `Artistic` way, and because the text is displayed in `ASCII Art` big letters! In the image above we can see our little black cat also listening to something, which is the sound of the letters contained in the book, since our reader also "reads for you" the text out loud in the language you select!

 For this `text-to-speech` feature our application uses the `Piper` engine in the background. You don't need to install `piper` or any of its libraries, because they are already embedded in the instalation.

## Why we show the texts in ASCII Art letters?

 Well, because it is "cool", it is "nice" and because it runs perfectly in any terminal showing any text in big beautiful letters.
 

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
 

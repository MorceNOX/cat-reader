# cat-reader
<img width="720" height="720" alt="image" src="https://github.com/MorceNOX/cat-reader/blob/main/assets/image_.png" />

 Choose your e-book, article or select any text and the reader will speak it out loud, sentence by sentence, while you read it in big ASCII Art letters!
 
 Navigate as you wish, and control everything easily: the speed, the language, the volume of the audio and even the colors!
 
 This is, by far, the most comfortable way to read a book and to be engaged in your favourite readings!

 `cat-reader` stands for C Art Text Reader, because is written in `C Language` and provides a way to read your `Text` in an `Artistic` way, and because the text is displayed in `ASCII Art` big letters! In the image above we can see our little black cat also listening to something, which is the sound of the letters contained in the book, since our reader also "reads for you" the text out loud in the language you select!

 For this `text-to-speech` feature our application uses the `Piper` engine in the background. You don't need to install `piper` or any of its libraries, because they are already embedded in the instalation.

## What does this application do?

 `cat-reader` is a text only reader. It simply reads any text you choose, be it a book, an article, or a selected piece of text in your browser or in any other application you may be using. 

 So, you choose a text in your screen by selecting it; or choose a text file; or a document in `epub`, `markdown`, `docx`, `rtf` and `html` formats, or in any other text format; or any `pdf` file which contais text and `cat-reader` will separate the provide text into sentences. Each created sentence will be read in the right order, one by one, beig displayed sequentially in big letters, in ASCII Art format and, at the same time, the same displayed sentence is spoken out loud in the language you have previously selected in the initial menu.

 While the reader displays and skeaks a sentence, a big bar, also displayed in ASCII Art letters, shows you the selected language, the speed of the reading, the volume of the audio and the ordinal number of the current sentence, together with the total number of sentences which the text contains.

 Simple controls, as the arrow keys, space bar, etc., allow you to navigate along the text, forward and backwards, sentence by sentence or in blocks. So you can "walk" and "jump" though the entire text as you press some keys.

 As you change the sentence by pressing a key, the current audio immediatelly stops an the new sentence is read, out loud and in big letters.

 You can also control the speed and the volume of the audio, as well, the colors which the ASCII Art text is displayed.

 Everything occurs quickly and easily, without consumming too much of your machine resources. For that the application is written in `C Language`.

## Why we show the texts in ASCII Art letters?

 Well, because it is "cool", it is "nice" and because it runs perfectly in any terminal showing any text in big beautiful letters. You can easily even control the zoom in and out using your terminal shortcuts (*generally `Ctrl +` or `Ctrl Shift +` to zoom in and `Ctrl -` to zoom out*).
 
## Requirements
 
- `gcc`
- `bash`
- `make`
- `pandoc`
- `poppler-utils`
- `jq`
- `newt`
- `alsa-utils`
- `json-c`

 You need a `gcc compiler`, `make` and `bash` to run the `Makefile` and generate the executable when you are building from source, which is the preferable method. It is simple and it guarantees that everything will work without issues on your personal machine and environment, since this application requires only libraries existing in any distro repository.

 In `Debian` or `Ubuntu` you type:
```
sudo apt install build-essential
```

 In `Fedora` or `CentOS`:
```
sudo dnf group install "Development Tools"
```
 or
```
sudo dnf intall gcc make
```
 
 `Bash` is also required to run the initial menu and configuration options.
 
 `Pandoc` is required for reading `.markdown` files, `.epub` files, `.docx` files and `.rtf` files.

 In `Debian` or `Ubuntu` you can install it by typing:
```
sudo apt install pandoc
```

 In `Fedora` or `CentOS` you just type:
```
sudo dnf install pandoc
```
 
 `Poppler-utils` is required for converting `.pdf` files into text, so you can read your .pdf files which contains text as if they were a simple plain text file.

 In `Debian` or `Ubuntu` you can install it by typing:
```
sudo apt install poppler-utils
```

 In `Fedora` or `CentOS` you just type:
```
sudo dnf install poppler-utils
```

 `jq` and `newt` is normally already installed on most ditros, but if not, you have to install these packages to make the application able to read configuration files and to display the menus properly. They are external common features used by the configuration options.

 In `Debian` or `Ubuntu` you can install it by typing:
```
sudo apt install jq newt
```

 In `Fedora` or `CentOS` you just type:
```
sudo dnf install jq newt
```

 `alsa-utils` may also be already installed in your system, or it can be easily installed from the standard repositories of any distro. It provides the background "audio-player" which our application uses to make the spoken texts audible.

 In `Debian` or `Ubuntu` you can install it by typing:
```
sudo apt install alsa-utils
```

 In `Fedora` or `CentOS` you just type:
```
sudo dnf install alsa-utils
```

 `json-c` is the only external `C` library required which may not be installed by default in your environment.

 In `Debian` or `Ubuntu` you can install it by typing:
```
sudo apt install libjson-c-dev
```

 In `Fedora` or `CentOS` you just type:
```
sudo dnf install json-c-dev
```
 
### Optional
- ascii-image-converter

 Just choose the version that fits your machine, extract the files and copy the binary to `/usr/local/bin`.
 
 Here is the link: [ascii-image-converter last release](https://github.com/TheZoraiz/ascii-image-converter/releases/tag/v1.13.1).
 
 This program is needed to show the image in *ASCII Art* format in the splash screen and when the application finnishes. It gives a little more *charm* to the reader, showing our little black cat reading a book, but if not installed, the Reader will still work normally.
 

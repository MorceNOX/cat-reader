# cat-reader
<img width="720" height="720" alt="image" src="https://github.com/MorceNOX/cat-reader/blob/main/assets/image_.png" />

 Choose your e-book, article or select any text and the reader will speak it out loud, sentence by sentence, while you read it in big ASCII Art letters!
 
 Navigate as you wish, and control everything easily: the speed, the language, the volume of the audio and even the colors!
 
 This is, by far, the most comfortable way to read a book and to be engaged in your favourite readings! You will *read* and *listen to* the text, at the same time, sentence by sentence.

## Why `cat-reader`?

 `cat-reader` stands for C Art Text Reader, because it is written in `C Language` and provides a way to read your *Text* in an *Artistic* way, and because the text is displayed in `ASCII Art` big letters (lots of unicode )! In the image above we can see our little black cat also listening to something, which is the sound of the letters contained in the book, since our reader also "reads for you" the text out loud in the language you select!

 For this `text-to-speech` feature our application uses the `Piper` engine in the background. You don't need to install `piper` or any of its libraries, because they are already embedded in the instalation.

## What does this application do?

 `cat-reader` is a text only reader. It reads an speaks any text you choose, be it a book, an article, or a selected piece of text in your browser or in any other application you may be using. 

 So, you choose a text in your screen by selecting it; or choose **a text file**; or **a document** in `epub`, `markdown`, `docx`, `odt`, `rtf` and `html` formats, or in **any other text format**; or **any `pdf` file which contais text** and `cat-reader` will separate the provided text into sentences,read them in the right order, one by one, and **displaying them sequentially in big letters**, in ASCII Art format and, at the same time, **the same displayed sentence is spoken out loud** in the language you have previously selected in the initial menu.

 While the reader displays and speaks a sentence, **a big bar**, also displayed in ASCII Art letters, shows you the selected language, the speed of the reading, the volume of the audio and the ordinal number of the current sentence, together with the total number of sentences which the whole text contains.

 Simple controls, as the arrow keys, space bar, etc., allow you **to navigate along the text**, forward and backwards, **sentence by sentence** or **in blocks**. So you can "walk" and "jump" through the entire text as you press one key.

 As you change the sentence by pressing a key, the current audio immediatelly stops an the next sentence is read, out loud, and dispplayed in big letters.

 You can also control the speed and the volume of the audio, as well, the colors which the ASCII Art text is displayed.

 The Escape key <ESC> or "Q" exit the reader and goes back to the main menu. Another <ESC> or "Q" exits the application. 

 Everything occurs quickly and easily, without consumming too much of your machine resources. For that the application is written in `C Language`.

## Why we show the texts in ASCII Art letters?

 Well, because it is "cool", it is "nice" and because it runs perfectly in any terminal, allowing to show any text in big beautiful letters, without requiring any external graphic tool or library. It is all just in your terminal! You can easily even control the zoom in and out using your terminal shortcuts (*generally `Ctrl +` or `Ctrl Shift +` to zoom in and `Ctrl -` to zoom out*).
 
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

 If it's not installed by default, you can run:

 In `Debian` or `Ubuntu`:
```
sudo apt-get install bash
```

 In `Fedora` or `CentOS`:
```
sudo dnf install bash
```

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

 `jq` and `newt` is normally already installed on most ditros, but if not, you have to install these packages to make the application able to read configuration files and to display the menus properly. They are external common features used by the configuration options. `jq` reads and parses `JSON` configuration files ant the `newt` package provides the command `whiptail` used to generate the sub-menus in this application. 

 In `Debian` or `Ubuntu` you install them by typing:
```
sudo apt install jq newt
```

 In `Fedora` or `CentOS` you type:
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
sudo dnf install json-c-devel
```

 Some of these required packages may require another set of packages, but the respective installation command will manage these additions properly.
 
### Optional Requirement
- ascii-image-converter

 Just choose the version that fits your machine, extract the files and copy the binary to `/usr/local/bin`.
 
 Here is the link to download it: [ascii-image-converter last release](https://github.com/TheZoraiz/ascii-image-converter/releases/tag/v1.13.1).
 
 This program is needed to show the cat image in *ASCII Art* format in the splash screen and when the application finnishes. It gives a little more *charm* to the reader, showing our little black cat reading a book, but if not installed, the Reader will still work normally.
 
## Installation

### 1. Clone this repository
```
git clone https://github.com/MorceNOX/cat-reader.git
```

### 2. Install the dependencies

#### 2.1 In Ubuntu / Debian based distros
```
sudo apt-get install build-essential bash pandoc poppler-utils jq newt alsa-utils libjson-c-dev
```
#### 2.2 In Fedora / CentOS / RedHat based distros
```
sudo dnf install gcc make bash pandoc poppler-utils jq newt alsa-utils json-c-devel
```

### 3. Build the Binary
```
cd cat-reader
make -j$(nproc)
make install-user
sudo make install
```

## Run the Application

Open your terminal and run:

```
cat-reader
```
Now you can intuitivelly navigate through the main menu, choose your language, config your voice models and select your documents or any text to be displayed in big letters and to be spoken out loud while you read the respective sentences.

## How to Use

Here are some screenshots followed by brief explanations. You can hit the **Help** option at any time in the **main menu** to get help.

### 1. The Main Menu
[00-initial_menu_screenshot.png](https://github.com/MorceNOX/cat-reader/blob/main/assets/00-initial_menu_screenshot.png)

### 2. The Reader Screen
<img width="1920" height="1154" alt="01-the_reader_in_default_colors_screenshot" src="https://github.com/cat-reader/blob/main/assets/01-initial_menu_screenshot.png" />

<img width="1920" height="1154" alt="02-the_reader_white_over_gray_screenshot" src="https://github.com/cat-reader/blob/main/assets/02-the_reader_white_over_gray_screenshot.png" />

<img width="1920" height="1154" alt="03-the_reader_yellow_over_blue_screenshot" src="https://github.com/cat-reader/blob/main/assets/03-the_reader_yellow_over_blue_screenshot.png" />

### 3. The File Selector
<img width="1920" height="1154" alt="04-the_file_selector_screenshot" src="https://github.com/cat-reader/blob/main/assets/04-the_file_selector_screenshot.png" />

### 4. The Language Selector
<img width="1920" height="1154" alt="05-the_language_selector_screenshot" src="https://github.com/cat-reader/blob/main/assets/05-the_language_selector_screenshot.png" />

### 5. The Language Selector
<img width="1920" height="1154" alt="06-the_voice_selector_screenshot" src="https://github.com/cat-reader/blob/main/assets/06-the_voice_selector_screenshot.png" />





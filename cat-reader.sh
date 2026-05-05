#!/usr/bin/env bash
#
#   MorceNOX CAT-Reader (C-Art Text Reader)™
#
#   Copyright (C) 2026 Amilcar Antonio Mesquita Rizk <amilcar.rizk@gmail.com>
#
#   This program is free software: you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program.  If not, see <https://www.gnu.org/licenses/>.
#

APP_NAME="cat-reader"
HOME_CONFIG="${XDG_CONFIG_HOME:-$HOME/.config}"

# O endereço exato será "injetado" aqui durante a instalação
INSTALL_PREFIX="/usr/local"

DATADIR="$INSTALL_PREFIX/share/$APP_NAME"
CONFIG_DIR="$HOME_CONFIG/$APP_NAME"

MODELS_DIR="$DATADIR/models"
OUT_TXT="$DATADIR/out_txt"

# Detect if cat-reader-engine is in PATH, otherwise use the local version
CORE_BINARY=$(command -v "${INSTALL_PREFIX}"/libexec/"${APP_NAME}"-engine 2>/dev/null || echo "./"${APP_NAME}"-engine")
ASCII_IMAGE_EXE=$(command -v "${INSTALL_PREFIX}"/ascii-image-converter 2>/dev/null || echo "./ascii-image-converter")
PY_SPLIT_EXE=$(command -v "${INSTALL_PREFIX}"/libexec/split_sentences 2>/dev/null || echo "./split_sentences.py")

# Define the list of options

APP_SLOGAN="MorceNOX Art Reader - Making your reading a state of art!"

create_centered_option() {
    local text="$1"
    local total_width=30  # Width of the menu box minus borders
    
    # Calculate how much padding we need
    local text_length=${#text}
    local padding_needed=$(( (total_width - text_length) / 2 ))
    local right_padding=$(( total_width - text_length - padding_needed ))
    
    # Create the padded option
    printf "%*s%s%*s" $padding_needed "" "$text" $right_padding ""
}

raw_options=("Read a File" 
             "Read the Selected Text" 
             "Format text file"  
             "Select the Language"
             "Configure a Voice" 
             "Get a Voice Model"
             "Setup New Languages" 
             "Help" 
             "Exit")

options=()
for option in "${raw_options[@]}"; do
    options+=("$(create_centered_option "$option")")
done
     
text=("Read a text file or a document." 
      "Select a text in your screen to read it." 
      "Divide the text by sentences, line by line." 
      "Select the language of the text to be read." 
      "Change the voice for a language."
      "Get a voice model file to the reader."
      "Configure and activate new languages."
      "Show the help screen."
      "Exit the program.")

msg=("Select a text file or a document. Documents will be converted to text." 
     "Select a text in your browser or another program and hit this option to read it." 
     "A text file will be divided by sentences, one sentence per line." 
     "Here you define the language of the text to be read." 
     "Choose and configure the default voice for a selected language."
     "Select a downloaded voice model file to be copied to the config directory."
     "The program will check for new voices and make the necessary configurations."
     "Show the help screen."
     "Exit the program.")

commands=("readfile"
          "read_selection"
          "format_text_file"
          "set_language"
          "config_voice"
          "get_voice"
          "setup_languages"
          "show_help"
          "cont=false")
      
      
current_selection=0 # Index of the currently highlighted option

language="en"
lang_set_by_user=false
cont=false

show_cursor() {
    tput civis
}

hide_cursor() {
    tput cvvis
}


cleanup() {
    set +f
    if [[ -f "$DATA_DIR"/out/output*.raw ]]; then
        rm "$DATA_DIR"/out/output*.raw
    fi

    if [[ -f "$OUT_TXT"/textfile ]]; then
        rm "$OUT_TXT"/textfile
    fi

    if [[ -f "$OUT_TXT"/textfile_sentences.txt ]]; then
        rm "$OUT_TXT"/textfile_sentences.txt
    fi
}


trap "cleanup; exit 0" EXIT TERM HUP
trap "cleanup" INT


print_ruler() {
    local width=$1    
    local char=$2

    printf -v SPACES "%*s" $width ""
    ruler="${SPACES// /${char}}"
    
    printf "%s" $ruler
}

print_bg_line() {
    local width=$1
    
    printf -v SPACES "%*s" $width ""
    printf "%s" "$SPACES"
}

print_status_bar() {
    local line=$1
    local menu_text="$2"

    for ((j=line; j< $NUM_ROWS - 4; j++)); do
        echo
    done

    echo -en ${COLOR_BAR}
    echo -n "┏"
    print_ruler $(( COLS - 3 )) "━"
    echo "┓"
        
    print_bg_line $(( COLS - 3 ))    
    printf "\r"

    

    echo -en "┃\e[1m Selected language:\e[22m\e[33;40m $language ${COLOR_BAR}┃ \e[1mText selected?\e[22m\e[33;40m $(if $TEXT_SELECTED; then echo "yes"; else echo "no "; fi) \e[22m${COLOR_BAR}┃ \e[1mMsg: \e[22m\e[3m${COLOR_TEXT_OPTION_BAR}$(if [[ "${menu_text}" == *"⚠️"* ]]; then echo -e " "; fi)${menu_text} ${COLOR_BAR}"
    print_bg_line $(( COLS - 54 - "${#menu_text}" - 1 ))
    echo -e "┃$COLOR_MENU_SHADOW${COLOR_BACKGROUND}▌"
    
    echo -en "${COLOR_BAR}┗"
    print_ruler $(( COLS - 3 )) "━"
    echo -e "┛$COLOR_MENU_SHADOW${COLOR_BACKGROUND}▌"

    echo -en $COLOR_MENU_SHADOW
    echo -en ${COLOR_BACKGROUND}
    echo -n " "
    
    echo -en ${COLOR_BACKGROUND}
    if [[ "$NUM_ROWS" -gt 32 ]]; then    
        print_ruler $(( COLS - 2 )) "█"  #"▀"
        echo -n "▌"  #"▘"
    else
        print_ruler $(( COLS - 2 )) "▀"
        echo -n "▘"
    fi
    
}


truncate_with_ellipsis() {
    local text="$1"
    local max_width="$2"
    local ellipsis="…"

    if [[ ${#text} -gt $((max_width - ${#ellipsis})) ]]; then
        printf "%.*s%s" $((max_width - ${#ellipsis})) "$text" "$ellipsis"
    else
        printf "%s" "$text"
    fi
}


file_select() {
    local current_dir="${1:-.}"
    
    shift
    local type=("$@")

    local selection
    local max_width=75
    local max_desc_width=22
    local ellipsis="…"

    while true; do
        local menu_items=()
        local directories=()
        local files=()
        local file_map=()  # This will map truncated names to full paths

        menu_items+=(".." "Go to parent directory")

        for item in "$current_dir"/* "$current_dir"/.*; do
            if [[ "$item" == "$current_dir/." || "$item" == "$current_dir/.." ]]; then
                continue
            fi

            if [[ -d "$item" ]]; then
                local display_name=$(basename "$item")
                local truncated_name=$(truncate_with_ellipsis "$display_name" $max_width)
                local desc="Directory"
                local truncated_desc=$(truncate_with_ellipsis "$desc" $max_desc_width)
                directories+=("$truncated_name" "$truncated_desc")
                # Store mapping for directories with absolute path
                local abs_path=$(cd "$item" && pwd)
                file_map+=("$truncated_name" "$abs_path")
            elif [[ -f "$item" ]]; then
                local display_name=$(basename "$item")
                local content_type=$(file -b --mime-type "$item")
                local truncated_name=$(truncate_with_ellipsis "$display_name" $max_width)
                local truncated_desc=$(truncate_with_ellipsis "$content_type" $max_desc_width)

                if [[ -n "$type" ]]; then
                    
                    for t in "${type[@]}"; do
                        if [[ "$content_type" == "$t"* ]]; then
                            files+=("$truncated_name" "$truncated_desc")
                            
                            # Store mapping for files with absolute path
                            local abs_path=$(cd "$current_dir" && pwd)/"$display_name"
                            file_map+=("$truncated_name" "$abs_path")
                        fi
                    done
                else
                    files+=("$truncated_name" "$truncated_desc")
                    
                    # Store mapping for files with absolute path
                    local abs_path=$(cd "$current_dir" && pwd)/"$display_name"
                    file_map+=("$truncated_name" "$abs_path")
                fi
            fi
        done

        menu_items+=("${directories[@]}")
        menu_items+=("${files[@]}")

        local abs_path=$(cd "$current_dir" && pwd)

        selection=$(LC_ALL="en_GB.UTF-8" whiptail --title "File Selector" \
                             --menu "Select a file or directory in $abs_path" \
                             --backtitle "$APP_SLOGAN" \
                             25 106 15 \
                             "${menu_items[@]}" \
                             3>&1 1>&2 2>&3)
        local exit_status=$?

        if [[ $exit_status -eq 0 ]]; then
            if [[ "$selection" == ".." ]]; then
                current_dir="$current_dir/.."
            else
                # Look up the full path using our map
                local full_path=""
                for ((i=0; i<${#file_map[@]}; i+=2)); do
                    if [[ "${file_map[i]}" == "$selection" ]]; then
                        full_path="${file_map[i+1]}"
                        break
                    fi
                done
                
                if [[ -d "$full_path" ]]; then
                    current_dir="$full_path"
                elif [[ -f "$full_path" ]]; then
                    echo "$full_path"
                    break
                fi
            fi
        else
            echo ""
            break
        fi
    done
}


set_language() {
    languages=($(ls "$MODELS_DIR"))
    if [ ${#languages[@]} -eq 0 ]; then
        MESSAGE="⚠️ No available language models!"
        cont=true
        return 1
    fi

    lang_menu=()

    for l in "${languages[@]}"; do
        lang_menu+=( "$l" " " )
    done
    
    language=$(LC_ALL="en_GB.UTF-8" whiptail --title "Language Selection" --menu "Language of thr Text" 15 30 8 \
                             --backtitle "$APP_SLOGAN" \
                            "${lang_menu[@]}" 3>&1 1>&2 2>&3)
                
    if [[ -z "$language" ]]; then language="en"; fi
    
    MESSAGE="⚠️ Language ${language} selected!"

    lang_set_by_user=true
    cont=true
}


get_voice() {
    local voice_file=$(file_select "." "application/octet-stream" "application/onnx")

    if [[ -z "$voice_file" ]]; then
        MESSAGE="⚠️ No file selected!"
        cont=true
        return 1
    fi

    local json_file="${voice_file}.json"
    if [[ ! -f "$json_file" ]]; then
        MESSAGE="⚠️ Voice configuration file not found!"
        cont=true
        return 1
    fi

    local voice_name="$(basename "${voice_file}")"

    local lang="${voice_name:0:2}"

    if [[ ! -d "$MODELS_DIR/$lang" ]]; then
        mkdir -p "$MODELS_DIR/$lang"
    fi

    cp "$voice_file" "$MODELS_DIR/$lang/"
    if [ $? -ne 0 ]; then
        MESSAGE="⚠️ Error copying voice file!"
        cont=true
        return 1
    fi

    cp "$json_file" "$MODELS_DIR/$lang/"
    if [ $? -ne 0 ]; then
        MESSAGE="⚠️ Error copying voice config file!"
        cont=true
        return 1
    fi

    setup_languages

    MESSAGE="⚠️ New voice ${voice_name} is now available!"
    cont=true
}


config_voice() {
    local lang="$1"

    # 1. select the language
    if [[ -z "$lang" ]]; then
        languages=($(ls "$MODELS_DIR"))
        if [ ${#languages[@]} -eq 0 ]; then
            MESSAGE="⚠️ No available language models!"
            cont=true
            return 1
        fi

        lang_menu=()

        for l in "${languages[@]}"; do
            lang_menu+=( "$l" " " )
        done
        
        lang=$(LC_ALL="en_GB.UTF-8" whiptail --title "Language Selection" --menu "Language to Configure" 15 30 8 \
                                --backtitle "$APP_SLOGAN" \
                                "${lang_menu[@]}" 3>&1 1>&2 2>&3)
                    
        if [[ -z "$lang" ]]; then
            MESSAGE="⚠️ No language selected!"
            cont=true
            return 1
        fi
    fi

    local target_env="$CONFIG_DIR/$lang.env"
    local target_model_dir="$MODELS_DIR/$lang"    
    
    # 2. selecting the voice model file
    models=($(ls "$target_model_dir"/*.onnx 2>/dev/null))
    if [ ${#models[@]} -eq 0 ]; then
        MESSAGE="⚠️ No models available for the language ${lang}!"
        cont=true
        return 1
    fi
    
    model_menu=()
    for k in "${!models[@]}"; do
        model_menu+=( "$k" "$(basename "${models[$k]}" .onnx)" )
    done

    local index=$(LC_ALL="en_GB.UTF-8" whiptail --title "Voice Model Selection" --menu "Select the voice for the language $lang" 15 60 8 \
                        --backtitle "$APP_SLOGAN" \
                        "${model_menu[@]}" 3>&1 1>&2 2>&3)
                
    if [[ -z "$index" ]]; then
        MESSAGE="⚠️ No voice model selected!"
        cont=true
        return 1
    fi

    local json_path="${models[$index]}.json"
    
    # 3. selecting the speaker
    local num_speakers=( $(cat "$json_path" | jq -r '.num_speakers' 2>/dev/null) )
    local speakers_key=( $(cat "$json_path" | jq -r '.speaker_id_map | to_entries[] | .key' 2>/dev/null) )
    local speakers_value=( $(cat "$json_path" | jq -r '.speaker_id_map | to_entries[] | .value' 2>/dev/null) )
    local speaker_menu=()

    local num_speaker=0

    if [[ "$num_speakers" -gt 1 ]]; then
        for n in "${!speakers_key[@]}"; do
            speaker_menu+=( "${speakers_value[$n]}" "${speakers_key[$n]}" )
        done

        num_speaker=$(LC_ALL="en_GB.UTF-8" whiptail --title "Voice Speaker Selection" --menu "Select the speaker for selected voice" 15 60 8 \
                        --backtitle "$APP_SLOGAN" \
                        "${speaker_menu[@]}" 3>&1 1>&2 2>&3)

    fi

    # 4. selecting the default speed
    local speed=1.0

    local speed_array=( 1.0 0.91 0.83 0.75 0.68 0.62 0.56 0.51 0.39 0.22 0.2 1.1 1.21 1.33 1.46 1.61 1.77 1.95 2.14 2.36 2.59 2.85 3.14 3.45 3.8 4.18 4.59 5.05 5.56 6.12)
    local speed_menu=()
    
    for (( nn=0; nn<"${#speed_array[@]}"; nn++)); do
        speed_menu+=( $(( nn + 1 )) "${speed_array[$nn]}" )
    done
    
    local speed_index=$(LC_ALL="en_GB.UTF-8" whiptail --title "Voice Speed Selection" --menu "Select the default speed for selected voice" 15 60 8 \
                        --backtitle "$APP_SLOGAN" \
                        "${speed_menu[@]}" 3>&1 1>&2 2>&3)

    if [[ -z "$speed_index" ]]; then
        speed_index=1
    fi

    speed_index=$(( speed_index * 2 - 1 ))
    speed="${speed_menu[$speed_index]}"


    # 5. selecting the default volume
    local volume=1.0

    local volume_array=( 1.0 0.0 0.4 0.5 0.6 0.7 0.8 0.9 1.5 2.0 2.5 3.0 3.5 4.0 4.5 5.0 5.5 6.0 7.0 )
    local volume_menu=()
    
    for (( nnn=0; nnn<"${#volume_array[@]}"; nnn++)); do
        volume_menu+=( $(( nnn + 1 )) "${volume_array[$nnn]}" )
    done
    
    local volume_index=$(LC_ALL="en_GB.UTF-8" whiptail --title "Voice Volume Selection" --menu "Select the default volume for selected voice" 15 60 8 \
                        --backtitle "$APP_SLOGAN" \
                        "${volume_menu[@]}" 3>&1 1>&2 2>&3)

    if [[ -z "$volume_index" ]]; then
        volume_index=1
    fi

    volume_index=$(( volume_index * 2 - 1 ))
    volume="${volume_menu[$volume_index]}"

    sed -i "s|^PIPER_VOICE_MODEL=.*|PIPER_VOICE_MODEL="${models[$index]}"|" "$target_env"
    sed -i "s|^PIPER_VOICE_JSON=.*|PIPER_VOICE_JSON="$json_path"|" "$target_env"
    sed -i "s|^SPEAKER_ID=.*|SPEAKER_ID="$num_speaker"|" "$target_env"
    sed -i "s|^DEFAULT_SPEED=.*|DEFAULT_SPEED="$speed"|" "$target_env"
    sed -i "s|^DEFAULT_VOLUME=.*|DEFAULT_VOLUME="$volume"|" "$target_env"

    MESSAGE="⚠️ Voice for the language ${lang} updated!"

    cont=true
}


setup_languages() {
    skel_file="$DATA_DIR/skel/skel.env"
    #local env_files=( $(ls "${CONFIG_DIR}"/*.env) )
    local languages=( $(ls "${MODELS_DIR}") )

    local config=false

    for l in "${languages[@]}"; do
        if [[ ! -f "$CONFIG_DIR/$l.env" ]]; then
            cp "$skel_file" "$CONFIG_DIR/$l.env"

            if ! config_voice "$l"; then
                MESSAGE="⚠️ Languages not configured!"
                cont=true
                return 1
            else
                config=true
            fi
        fi
    done

    if $config; then    
        MESSAGE="⚠️ New languages succesfuly configured!"
    else
        MESSAGE="⚠️ Languages were already configured!"
    fi

    cont=true
}


read_selection() {
    if [[ -z "$(wl-paste -p 2>/dev/null)" ]]; then
        MESSAGE="⚠️ No text selected!"
        cont=true
        return 1
    fi

    set -f
    echo $(wl-paste -p 2>/dev/null) | sed -e 's/)/\)/g' -e 's/(/\(/g' -e "s/'/\\'/g" -e 's/\"/\\\"/g' | "$CORE_BINARY" -l $language 2>/dev/null
    set +f
    MESSAGE="⚠️ Thanks for reading!"
    cont=true
    sleep 2
}

readfile() {
    txtfile=$(file_select "." "text" "application/epub" "application/pdf" "application/vnd.openxmlformats-officedocument.wordprocessingml.document")
    
    if [[ -z "$txtfile" ]]; then
        MESSAGE="⚠️ No file selected!"
        cont=true
        return 1
    fi
    
    if ! $language_set_by_user; then
        set_language
    fi
    
    content_type=$(file -b --mime-type "${txtfile}")
    
    if [[ "$content_type" == *epub* ]]; then        
        pandoc -f epub -t plain --wrap=none --toc=false "$txtfile" -o "$OUT_TXT"/textfile
        if [ $? -ne 0 ]; then
            MESSAGE="⚠️ Error converting epub to text!"
            cont=true
            return 1
        fi

        "$PY_SPLIT_EXE" "$OUT_TXT"/textfile -o "$OUT_TXT"/textfile_sentences.txt
        if [ $? -ne 0 ]; then
            MESSAGE="⚠️ Error splitting sentences!"
            cont=true
            return 1
        fi

        "$CORE_BINARY" -l $language "$OUT_TXT"/textfile_sentences.txt 2>/dev/null
        MESSAGE="⚠️ Thanks for reading!"

        cont=true
        sleep 2
        return 0
    
    elif [[ "$content_type" == *markdown* ]]; then
        pandoc -f markdown -t plain --wrap=none --toc=false "$txtfile" -o "$OUT_TXT"/textfile
        if [ $? -ne 0 ]; then
            MESSAGE="⚠️ Error converting markdown to text!"
            cont=true
            return 1
        fi

        "$PY_SPLIT_EXE" "$OUT_TXT"/textfile -o "$OUT_TXT"/textfile_sentences.txt
        if [ $? -ne 0 ]; then
            MESSAGE="⚠️ Error splitting sentences!"
            cont=true
            return 1
        fi

        "$CORE_BINARY" -l $language "$OUT_TXT"/textfile_sentences.txt 2>/dev/null

        MESSAGE="⚠️ Thanks for reading!"

        cont=true
        sleep 2
        return 0

    elif [[ "$content_type" == *pdf* ]]; then
        pdftotext -q -nopgbrk -enc UTF-8 -eol unix "$txtfile" "$OUT_TXT"/textfile
        if [ $? -ne 0 ]; then
            MESSAGE="⚠️ Error converting pdf to text!"
            cont=true
            return 1
        fi

        "$PY_SPLIT_EXE" "$OUT_TXT"/textfile -o "$OUT_TXT"/textfile_sentences.txt
        if [ $? -ne 0 ]; then
            MESSAGE="⚠️ Error splitting sentences!"
            cont=true
            return 1
        fi

        "$CORE_BINARY" -l $language "$OUT_TXT"/textfile_sentences.txt 2>/dev/null
        MESSAGE="⚠️ Thanks for reading!"

        cont=true
        sleep 2
        return 0

    elif [[ "$content_type" == *rtf* ]]; then
        pandoc -s -f rtf -t plain --wrap=none --toc=false "$txtfile" -o "$OUT_TXT"/textfile
        if [ $? -ne 0 ]; then
            MESSAGE="⚠️ Error converting rtf to text!"
            cont=true
            return 1
        fi

        "$PY_SPLIT_EXE" "$OUT_TXT"/textfile -o "$OUT_TXT"/textfile_sentences.txt
        if [ $? -ne 0 ]; then
            MESSAGE="⚠️ Error splitting sentences!"
            cont=true
            return 1
        fi

        "$CORE_BINARY" -l $language "$OUT_TXT"/textfile_sentences.txt 2>/dev/null

        MESSAGE="⚠️ Thanks for reading!"

        cont=true
        sleep 2
        return 0

    elif [[ "$content_type" == *vnd.openxmlformats-officedocument.wordprocessingml.document* ]]; then
        pandoc -s -f docx -t plain --wrap=none --toc=false "$txtfile" -o "$OUT_TXT"/textfile
        if [ $? -ne 0 ]; then
            MESSAGE="⚠️ Error converting docx to text!"
            cont=true
            return 1
        fi

        "$PY_SPLIT_EXE" "$OUT_TXT"/textfile -o "$OUT_TXT"/textfile_sentences.txt
        if [ $? -ne 0 ]; then
            MESSAGE="⚠️ Error splitting sentences!"
            cont=true
            return 1
        fi

        
        "$CORE_BINARY" -l $language "$OUT_TXT"/textfile_sentences.txt 2>/dev/null

        MESSAGE="⚠️ Thanks for reading!"

        cont=true
        sleep 2
        return 0
    fi
    
    "$CORE_BINARY" -l $language "$txtfile" 2>/dev/null
    MESSAGE="⚠️ Thanks for reading!"
    
    cont=true
    sleep 2
}


format_text_file() {
    txtfile=$(file_select "." "text")
    
    if [[ -z "$txtfile" ]]; then
        MESSAGE="⚠️ No file selected!"
        cont=true
        return 1
    fi
    
    "$PY_SPLIT_EXE" "$txtfile" -o "${txtfile%.*}"_sentences.txt

    if [[ -z "${txtfile%.*}"_sentences.txt ]] then
       MESSAGE="⚠️ No file generated! Check for errors!"
       cont=true
       sleep 2
       return 1 
    fi
        
    MESSAGE="⚠️ File ${txtfile%.*}_sentences.txt generated!"
    cont=true
    sleep 2
}


show_help() {
    local help_str+="$(cat "${DATA_DIR}/help.txt")"

    LANG="en_GB.UTF-8" && LC_ALL="en_GB.UTF-8" && \
    whiptail --title "MorceNOX Art Reader - Help Screen!" \
             --msgbox "${help_str}" \
             --backtitle "$APP_SLOGAN" \
             --scrolltext \
             25 80

    cont=true
}

CODE_COLOR_BACKGROUND=44
CODE_COLOR_BAR_BACKGROUND=100
CODE_COLOR_MENU_FOREGROUND=30
CODE_COLOR_MENU_BACKGROUND=47

COLOR_BACKGROUND="\e[48;5;19m"

COLOR_LOGO="\e[1;32m"
COLOR_SLOGAN="\e[2;32m"
COLOR_TEXT_OPTION="\e[0;33m"
COLOR_MENU_SHADOW="\e[38;5;235m"

COLOR_LOGO_AND_BACK="\e[1;32;${CODE_COLOR_BACKGROUND}m"
COLOR_SLOGAN_AND_BACK="\e[2;32;${CODE_COLOR_BACKGROUND}m"
COLOR_TEXT_OPTION_AND_BACK="\e[0;33;${CODE_COLOR_BACKGROUND}m"
COLOR_MENU_FRAME="\e[2;30;${CODE_COLOR_MENU_BACKGROUND}m"
COLOR_MENU_SHADOW_AND_BACK="\e[0;30;${CODE_COLOR_BACKGROUND}m"
COLOR_MENU_OPTION="\e[0;${CODE_COLOR_MENU_FOREGROUND};${CODE_COLOR_MENU_BACKGROUND}m"
COLOR_MENU_OPTION2="\e[0;31;47m" # The color set to be inverted 
COLOR_BAR="\e[37;${CODE_COLOR_BAR_BACKGROUND}m"
COLOR_TEXT_OPTION_BAR="\e[32;${CODE_COLOR_BAR_BACKGROUND}m"

LOGO=("                 ▖  ▖        ▖ ▖▄▖▖▖TM"
      "                 ▛▖▞▌▛▌▛▘▛▘█▌▛▖▌▌▌▚▘  "
      " ▗▄▄▖      ▗▄▄▄▖ ▌▝ ▌▙▌▌ ▙▖▙▖▌▝▌▙▌▌▌  "
      "▐▌    ┏┓     █     ▗  ▄▖     ▌        "
      "▐▌    ┣┫┏┓╋  █ █▌▚▘▜▘ ▙▘█▌▀▌▛▌█▌▛▘    "
      "▝▚▄▄▖ ┛┗┛ ┗  █ ▙▖▞▖▐▖ ▌▌▙▖█▌▙▌▙▖▌     ")

SLOGAN=("╭┬╮╭─╮╷╭ ╷╭╮╷╭─╴   ╶┬╴╷ ╷╭─╴   ╭─╮╭─╴╭─╮╶┬╮╷╭╮╷╭─╴   ╭─╮   ╭─╮╶┬╴╭─╮╶┬╴╭─╴   ╭─╮╭─╴   ╭─╮╭─╮╶┬╴"
        "┃┃┃┣━┫┣┻┓┃┃┗┫┃╺┓    ┃ ┣━┫┣╸    ┣┳┛┣╸ ┣━┫ ┃┃┃┃┗┫┃╺┓   ┣━┫   ┗━┓ ┃ ┣━┫ ┃ ┣╸    ┃ ┃┣╸    ┣━┫┣┳┛ ┃ "
        "╹ ╹╹ ╹╹ ╹╹╹ ╹┗━┛    ╹ ╹ ╹┗━╸   ╹┗╸┗━╸╹ ╹╺┻┛╹╹ ╹┗━┛   ╹ ╹   ┗━┛ ╹ ╹ ╹ ╹ ┗━╸   ┗━┛╹     ╹ ╹╹┗╸ ╹ ")

# Function to display the menu
display_menu() {

    echo -en "${COLOR_LOGO}"
    echo -en "$COLOR_BACKGROUND"
    
    # Clear the screen
    clear

    echo
    echo

    LINE=3

    # printing the logo
    local padding=$(( $(( COLS - "${#LOGO[0]}" )) / 2 ))
    local pad_str=""
    for ((i=0; i<padding; i++)); do pad_str+=" "; done

    for ((j=0; j<"${#LOGO[@]}"; j++)); do
        echo  "${pad_str}${LOGO[$j]}"
        LINE=$(( LINE + 1 ))
    done

    echo
    LINE=$(( LINE + 1 ))
    
    # printing the slogan ascii art
    padding=$(( $(( COLS - "${#SLOGAN[0]}" )) / 2 ))
    pad_str=""
    for ((i=0; i<padding; i++)); do pad_str+=" "; done

    echo -en "${COLOR_SLOGAN}"    
    for ((jj=0; jj<"${#SLOGAN[@]}"; jj++)); do
        echo  "${pad_str}${SLOGAN[$jj]}"
        LINE=$(( LINE + 1 ))
    done
    
    echo

    LINE=$(( LINE + 1 ))

    
    # printing the menu box
    padding=$(( $(( COLS - 38 )) / 2 ))
    pad_str=""
    for ((i=0; i<padding; i++)); do pad_str+=" "; done

    echo -e "${pad_str}${COLOR_MENU_FRAME}┌───────────────────────────────────┐"
    
    LINE=$(( LINE + 1 ))

    for i in "${!options[@]}"; do                   
        echo -en "${COLOR_BACKGROUND}${pad_str}${COLOR_MENU_FRAME}│ ${COLOR_MENU_OPTION2}"
        
        if [[ $i -eq $current_selection ]]; then
            echo -en "\e[7m\e[1m> ${options[$i]} \e[0m" # Highlighted option (inverted colors)
                        
        else
            echo -en "${COLOR_MENU_OPTION}  ${options[$i]} \e[0m" # Regular option
            
        fi
        echo -e "${COLOR_MENU_FRAME} │${COLOR_MENU_SHADOW}${COLOR_BACKGROUND}█"
        LINE=$(( LINE + 1))
    done
    
    echo -e "${pad_str}${COLOR_MENU_FRAME}└───────────────────────────────────┘${COLOR_MENU_SHADOW}${COLOR_BACKGROUND}█"
    echo -e "${pad_str} █████████████████████████████████████"
    LINE=$(( LINE + 2))
    
    echo

    padding=$(( $(( COLS - "${#msg[$current_selection]}" )) / 2 ))
    pad_str=""
    for ((i=0; i<padding; i++)); do pad_str+=" "; done
    echo -e "${pad_str}${COLOR_TEXT_OPTION_AND_BACK}${COLOR_BACKGROUND}${msg[$current_selection]}"
    
    echo
    LINE=$(( LINE + 3))
        
    print_status_bar $LINE "${MESSAGE}"
        
}

MESSAGE="${text[0]}"

clear

"$ASCII_IMAGE_EXE" "${CONFIG_DIR}/image.png" -b --threshold 96 -C --color-bg 2>/dev/null
echo "$APP_SLOGAN"

sleep 3

while true; do
    TEXT_SELECTED=false
    if [[ -n "$(wl-paste -p 2>/dev/null)" ]]; then
        TEXT_SELECTED=true
    else
        TEXT_SELECTED=false
    fi

    COLS=$(tput cols)
    NUM_ROWS=$(tput lines)
    
    display_menu

    # Read a single character input without echoing
    read -rsn1 input

    # Check for arrow key input (escape sequences)
    if [[ "$input" == $'\x1b' ]]; then # Escape sequence starts with ESC (0x1b)
        # We need to peek at the next characters to handle repeated sequences properly
        # Use a timeout to read the next characters
        read -rsn2 -t 0.1 input # Try to read the next 2 characters with a short timeout
        
        case "$input" in
            '[A'*) # Up arrow
                ((current_selection--))
                if [[ $current_selection -lt 0 ]]; then
                    current_selection=$(( ${#options[@]} - 1 )) # Wrap around to bottom
                fi
                MESSAGE="${text[$current_selection]}"
                ;;
            '[B'*) # Down arrow
                ((current_selection++))
                if [[ $current_selection -ge ${#options[@]} ]]; then
                    current_selection=0 # Wrap around to top
                fi
                MESSAGE="${text[$current_selection]}"
                ;;
            '[C') # Right arrow
                # Add right arrow handling if needed
                ;;
            '[D') # Left arrow
                # Add left arrow handling if needed
                ;;
            '')
                break
                ;;
            $'\x1b'*)
                break
                ;;
            *) # Handle other escape sequences or just ignore them
                # For repeated keys, we might get extra characters that need to be handled
                # If it's not a known arrow key, just continue
                ;;
        esac
    elif [[ "$input" == "" ]]; then # Enter key
        echo -en "\e[0m"
        echo
        
        eval ${commands[$current_selection]}
                
        if $cont; then
            continue
        fi
        break # Exit the loop
    elif [[ "$input" == "q" ]]; then # 'q' to quit
        break
    fi
done

echo -en "\e[0m"
echo -e "\nExiting."

sleep 2

clear
"$ASCII_IMAGE_EXE" "${CONFIG_DIR}/image.png" -f -b --threshold 96 -C --color-bg 2>/dev/null
echo "$APP_SLOGAN Thanks for reading with us!"
echo
echo "    MorceNOX CAT-Reader (C-Art Text Reader)™ Copyright © 2026  Amilcar Antonio Mesquita Rizk"
echo "    This program comes with ABSOLUTELY NO WARRANTY; for details go to the 'help' option."
echo "    This is free software, and you are welcome to redistribute it"
echo "    under certain conditions; see <https://www.gnu.org/licenses/> for details."
echo
exit 0


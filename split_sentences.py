#! /bin/python
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

import re
import sys
import os
import argparse

def split_sentences_to_file(input_path, output_path):
    if not os.path.exists(input_path):
        print(f"Error: File '{input_path}' not found.")
        return

    with open(input_path, 'r', encoding='utf-8') as f:
        # Read the file and replace existing newlines with spaces 
        # to treat paragraphs as single continuous strings.
        content = f.read().replace('\n', ' ')

    # This regex looks for a period, exclamation, or question mark
    # followed by one or more whitespace characters.
    # The (?<=[.!?;:]) is a "positive lookbehind" - it checks if the 
    # preceding character is punctuation, but doesn't include it in the split.
    sentences = re.split(r'(?<=[.!?;:”\"\]\)-])\s+', content)

    with open(output_path, 'w', encoding='utf-8') as f:
        for sentence in sentences:
            clean_sentence = sentence.strip()
            if clean_sentence:
                f.write(clean_sentence + '\n')

    print(f"Success! Processed text saved to: {output_path}")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Split a text file into individual sentences.")
    
    # Positional argument: The input file
    parser.add_argument("input_file", help="Path to the input text file.")
    
    # Optional argument: The output file path
    parser.add_argument("-o", "--output", help="Path to the output file. If not provided, a default name is generated.")

    args = parser.parse_args()

    input_file = args.input_file
    
    # Check if the user provided a custom output path
    if args.output:
        output_file = args.output
    else:
        # Default behavior: Creates an output file name based on the input name
        output_file = os.path.splitext(input_file)[0] + "_sentences.txt"
    
    split_sentences_to_file(input_file, output_file)

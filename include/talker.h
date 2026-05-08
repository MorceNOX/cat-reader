/*
* MorceNOX C Art Text Reader™
*
* Copyright (C) 2026 Amilcar Antonio Mesquita Rizk <amilcar.rizk@gmail.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef TALKER_H
#define TALKER_H

int extract_sample_rate(const char* json_path, int default_value);
float extract_length_scale(const char* json_path, double default_value);
void scale_audio_samples(float* samples, size_t num_samples, float volume);
void convert_f32le_to_s16le(FILE *input, FILE *output);
int play_audio(const char* filename, int sample_rate);
int talker(const char* model_path, const char* json_path, const int speaker, const char* espeak_path, float speed, char* text, float volume);
#endif

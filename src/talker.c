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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <piper.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <pthread.h>
#include <SDL2/SDL.h>
#include "talker.h"
#include "json_helper.h"
#include "environment.h"

// Global mutex for file operations
//static pthread_mutex_t file_mutex = PTHREAD_MUTEX_INITIALIZER;

int extract_sample_rate(const char* json_path, int default_value) {
    const char* path_parts[] = {"audio", "sample_rate"};
    return (int)json_get_numeric_value(json_path, path_parts, 2, (double)default_value);
}


float extract_length_scale(const char* json_path, double default_value) {
    const char* path_parts[] = {"inference", "length_scale"};
    return (float)json_get_numeric_value(json_path, path_parts, 2, default_value);
}


void scale_audio_samples(float* samples, size_t num_samples, float volume) {
    // Handle extreme volume values
    if (volume < 0.0f) volume = 0.0f;  // Prevent negative volumes
    
    // Apply volume scaling
    for (size_t i = 0; i < num_samples; i++) {
        samples[i] *= volume;
        
        // Clamp to avoid overflow, but be more aggressive with volumes > 1.0
        if (volume > 1.0f) {
            // For volumes > 1.0, we're more aggressive about clamping
            if (samples[i] > 1.0f) samples[i] = 1.0f;
            if (samples[i] < -1.0f) samples[i] = -1.0f;
        } else {
            // For volumes <= 1.0, just make sure we don't go over
            if (samples[i] > 1.0f) samples[i] = 1.0f;
            if (samples[i] < -1.0f) samples[i] = -1.0f;
        }
    }
}



void convert_f32le_to_s16le(FILE *input, FILE *output) {
    float f_sample;
    while (fread(&f_sample, sizeof(float), 1, input) == 1) {
        // Normalize and clamp
        float s_sample = f_sample * 32767.0f;
        if (s_sample > 32767.0f) s_sample = 32767.0f;
        if (s_sample < -32768.0f) s_sample = -32768.0f;

        int16_t i_sample = (int16_t)s_sample;
        fwrite(&i_sample, sizeof(int16_t), 1, output);
    }
}

// Function to play audio with correct sample rate
int play_audio(const char* filename, int sample_rate) {
    // Create command with proper parameters
    char command[512];
    // aplay -r 22050 -c 1 -f FLOAT_LE -t raw output.raw
    snprintf(command, sizeof(command), "aplay -c %d -r %d -f FLOAT_LE -t raw %s", 1, sample_rate, filename);
    
    int status = system(command);
    if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
        return 1; // Success
    }
    
    // Fallback to ffplay if aplay fails
    // ffplay -f f32le -ar 22050 -i arquivo_ou_pipe
    snprintf(command, sizeof(command), "ffplay -nodisp -hide_banner -autoexit -f f32le -ar %d -i %s", sample_rate, filename);
    status = system(command);
    
    return WIFEXITED(status) && WEXITSTATUS(status) == 0;
}

// Thread-safe talker function
int talker(
    const char* model_path, 
    const char* json_path, 
    const int speaker, 
    const char* espeak_path, 
    float speed, 
    char* text,
    float volume
) {
    // 1. Initialize Synthesizer and extract metadata
    piper_synthesizer *synth = piper_create(model_path, json_path, espeak_path);
    if (!synth) {
        fprintf(stderr, "Error: Failed to create synthesizer\n");
        return 1;
    }
    
    float length_scale = extract_length_scale(json_path, 1.0);
    if (length_scale <= 0) {
        fprintf(stderr, "Warning: Could not extract length scale, using default 1.0\n");
        length_scale = 1.0;
    }
        
    // Get sample rate from JSON file
    int sample_rate = extract_sample_rate(json_path, 22050);
    if (sample_rate <= 0) {
        fprintf(stderr, "Warning: Could not extract sample rate, using default 22050\n");
        sample_rate = 22050;
    }
    
    float phoneme_length = length_scale / speed;

    // 2. Initialize SDL2 Audio
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        piper_free(synth);
        return 1;
    }

    SDL_AudioSpec want, have;
    SDL_zero(want);
    want.freq = sample_rate;
    want.format = AUDIO_F32;
    want.channels = 1;
    want.samples = 4096;     // Buffer size

    SDL_AudioDeviceID dev = SDL_OpenAudioDevice(NULL, 0, &want, &have, 0);
    if (dev == 0) {
        fprintf(stderr, "Failed to open audio device: %s\n", SDL_GetError());
        SDL_Quit();
        piper_free(synth);
        return 1;
    }

    // IMPORTANT: Unpause the device to start playback
    SDL_PauseAudioDevice(dev, 0);
    
    // 3. Set up synthesis options
    piper_synthesize_options options = piper_default_synthesize_options(synth);
    // Change options here:
    options.length_scale = phoneme_length;
    options.speaker_id = speaker;
    
    // 4. Start synthesis
    piper_synthesize_start(synth, text, &options);
    
    // 5. The streaming loop. Process audio chunks
    piper_audio_chunk chunk;
    while (piper_synthesize_next(synth, &chunk) != PIPER_DONE) {
        // Scale the samples before playing
        scale_audio_samples((float*)chunk.samples, chunk.num_samples, volume);

        // Push the chunk directly to the SDL audio queue
        // We multiply num_samples by sizeof(float) to get the total byte size
        SDL_QueueAudio(dev, chunk.samples, chunk.num_samples * sizeof(float));
    }    

    // 6. Wait for the buffer to finish playing
    // Since SDL_QueueAudio is asynchronous, the loop finishes as soon as 
    // the last chunk is queued, NOT when it finishes playing.
    // We must wait until the queue size reaches zero.
    while (SDL_GetQueuedAudioSize(dev) > 0) {
        SDL_Delay(10); // Small sleep to prevent 100% CPU usage while waiting
    }

    // 7. Cleanup
    SDL_CloseAudioDevice(dev);
    SDL_Quit();
    piper_free(synth);

    return 0;
}
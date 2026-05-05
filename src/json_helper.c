#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <json-c/json.h>
#include "json_helper.h"

char* json_get_string(const char* json_path, const char* json_path_parts[], int num_parts, char* default_value) {
    if (!json_path) return default_value;
    
    FILE *file = fopen(json_path, "r");
    if (!file) {
        fprintf(stderr, "Warning: Could not open JSON file %s\n", json_path);
        return default_value;
    }
    
    // Read entire file into memory
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    char *json_data = malloc(file_size + 1);
    if (!json_data) {
        fclose(file);
        return default_value;
    }
    
    fread(json_data, 1, file_size, file);
    json_data[file_size] = '\0';
    fclose(file);
    
    // Parse JSON
    json_object *root = json_tokener_parse(json_data);
    if (!root) {
        free(json_data);
        return default_value;
    }
    
    // Navigate to the specified path
    json_object *current_obj = root;
    for (int i = 0; i < num_parts; i++) {
        current_obj = json_object_object_get(current_obj, json_path_parts[i]);
        if (!current_obj) {
            json_object_put(root);
            free(json_data);
            return default_value;
        }
    }
    
    // Extract the string value
    const char *string_value = json_object_get_string(current_obj);
    if (!string_value) {
        json_object_put(root);
        free(json_data);
        return default_value;
    }
    
    // Allocate memory for the returned string and copy it
    char *result = malloc(strlen(string_value) + 1);
    if (!result) {
        json_object_put(root);
        free(json_data);
        return default_value;
    }
    
    strcpy(result, string_value);
    json_object_put(root);
    free(json_data);
    
    return result;
}


double json_get_numeric_value(const char* json_path, const char* json_path_parts[], int num_parts, double default_value) {
    if (!json_path) return default_value;
    
    FILE *file = fopen(json_path, "r");
    if (!file) {
        fprintf(stderr, "Warning: Could not open JSON file %s\n", json_path);
        return default_value;
    }
    
    // Read entire file into memory
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    char *json_data = malloc(file_size + 1);
    if (!json_data) {
        fclose(file);
        return default_value;
    }
    
    fread(json_data, 1, file_size, file);
    json_data[file_size] = '\0';
    fclose(file);
    
    // Parse JSON
    json_object *root = json_tokener_parse(json_data);
    if (!root) {
        free(json_data);
        return default_value;
    }
    
    // Navigate to the specified path
    json_object *current_obj = root;
    for (int i = 0; i < num_parts; i++) {
        current_obj = json_object_object_get(current_obj, json_path_parts[i]);
        if (!current_obj) {
            json_object_put(root);
            free(json_data);
            return default_value;
        }
    }
    
    // Get the numeric value (handle both int and double)
    double result = 0.0;
    enum json_type type = json_object_get_type(current_obj);
    switch (type) {
        case json_type_int:
            result = (double)json_object_get_int(current_obj);
            break;
        case json_type_double:
            result = json_object_get_double(current_obj);
            break;
        case json_type_boolean:
            result = json_object_get_boolean(current_obj) ? 1.0 : 0.0;
            break;
        default:
            result = default_value;
    }
    
    json_object_put(root);
    free(json_data);
    
    return result;
}
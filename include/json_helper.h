#ifndef JSON_HELPER_H_
#define JSON_HELPER_H_

char* json_get_string(const char* json_path, const char* json_path_parts[], int num_parts, char* default_value);
double json_get_numeric_value(const char* json_path, const char* json_path_parts[], int num_parts, double default_value);
#endif // JSON_HELPER_H_
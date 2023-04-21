int main();
int process_bw_image();
int process_rgb_image(char* in_path, char* out_path);
void str_lower_case(char* str);
void* thread_start(void* arg);
int is_jpg_file(const char* file_name);
char** get_jpg_files(const char* dir_path, int* num_files);

#pragma once

#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <regex>

#include <qstandarditemmodel.h>

#include <mupdf/fitz.h>

#include "book.h"
#include "utf8.h"


#define LL_ITER(name, start) for(auto name=start;(name);name=name->next)

std::wstring to_lower(const std::wstring& inp);
void convert_toc_tree(fz_outline* root, std::vector<TocNode*>& output, fz_context* context, fz_document* doc);
bool is_separator(fz_stext_char* last_char, fz_stext_char* current_char);
void get_flat_toc(const std::vector<TocNode*>& roots, std::vector<std::wstring>& output, std::vector<int>& pages);
int mod(int a, int b);
bool intersects(float range1_start, float range1_end, float range2_start, float range2_end);
void parse_uri(std::string uri, int* page, float* offset_x, float* offset_y);
bool includes_rect(fz_rect includer, fz_rect includee);
char get_symbol(int scancode, bool is_shift_pressed);
//GLuint LoadShaders(filesystem::path vertex_file_path_, filesystem::path fragment_file_path_);

template<typename T>
int argminf(const std::vector<T> &collection, std::function<float(T)> f) {

	float min = std::numeric_limits<float>::infinity();
	int min_index = -1;
	for (int i = 0; i < collection.size(); i++) {
		float element_value = f(collection[i]);
		if (element_value < min){
			min = element_value;
			min_index = i;
		}
	}
	return min_index;
}
void rect_to_quad(fz_rect rect, float quad[8]);
void copy_to_clipboard(const std::wstring& text);
fz_rect corners_to_rect(fz_point corner1, fz_point corner2);
void install_app(const char* argv0);
int get_f_key(std::string name);
void show_error_message(const std::wstring& error_message);
std::wstring utf8_decode(const std::string& encoded_str);
std::string utf8_encode(const std::wstring& decoded_str);
// is the character a right to left character
bool is_rtl(int c);
std::wstring reverse_wstring(const std::wstring& inp);
bool parse_search_command(const std::wstring& search_command, int* out_begin, int* out_end, std::wstring* search_text);
QStandardItemModel* get_model_from_toc(const std::vector<TocNode*>& roots);

// given a tree of toc nodes and an array of indices, returns the node whose ith parent is indexed by the ith element
// of the indices array. That is:
// root[indices[0]][indices[1]] ... [indices[indices.size()-1]]
TocNode* get_toc_node_from_indices(const std::vector<TocNode*>& roots, const std::vector<int>& indices);

//fz_stext_char* find_closest_char_to_document_point(fz_stext_page* stext_page, fz_point document_point, int* location_index);
fz_stext_char* find_closest_char_to_document_point(const std::vector<fz_stext_char*> flat_chars, fz_point document_point, int* location_index);
void get_stext_block_string(fz_stext_block* block, std::wstring& res);
void get_stext_page_string(fz_stext_page* page, std::wstring& res);
bool does_stext_block_starts_with_string(fz_stext_block* block, const std::wstring& str);
bool does_stext_block_starts_with_string_case_insensitive(fz_stext_block* block, const std::wstring& str);
std::wstring get_figure_string_from_raw_string(const std::wstring& raw_string);
void merge_selected_character_rects(const std::vector<fz_rect>& selected_character_rects, std::vector<fz_rect>& resulting_rects);
void string_split(std::string haystack, const std::string& needle, std::vector<std::string>& res);
void run_command(std::wstring command, std::wstring parameters);

std::wstring get_string_from_stext_line(fz_stext_line* line);
std::wstring get_string_from_stext_block(fz_stext_block* block);
void sleep_ms(unsigned int ms);
void open_url(const std::string& url_string);
void open_url(const std::wstring& url_string);
void open_file(const std::filesystem::path& path);
void search_google_scholar(const std::wstring& search_string);
void search_libgen(const std::wstring& search_string);
void index_references(fz_stext_page* page, int page_number, std::map<std::wstring, IndexedData>& indices);
void get_flat_chars_from_stext_page(fz_stext_page* stext_page, std::vector<fz_stext_char*>& flat_chars);
int find_best_vertical_line_location(fz_pixmap* pixmap, int relative_click_x, int relative_click_y);
//void get_flat_chars_from_stext_page_with_space(fz_stext_page* stext_page, std::vector<fz_stext_char*>& flat_chars, fz_stext_char* space);
void index_equations(const std::vector<fz_stext_char*>& flat_chars, int page_number, std::map<std::wstring, IndexedData>& indices);
void find_regex_matches_in_stext_page(const std::vector<fz_stext_char*>& flat_chars,
	const std::wregex& regex,
	std::vector<std::pair<int, int>>& match_ranges, std::vector<std::wstring>& match_texts);
bool is_string_numeric(const std::wstring& str);
void create_file_if_not_exists(const std::filesystem::path& path);

#pragma once
#include <vector>
#include <string>
#include <optional>
#include <iostream>
#include <thread>
#include <map>

//#include <Windows.h>
#include <qstandarditemmodel.h>
#include <qdatetime.h>

#include <qobject.h>

#include <mupdf/fitz.h>
#include "sqlite3.h"

#include "database.h"
#include "utils.h"
#include "book.h"

#ifdef __APPLE__
  #include <unordered_map>
#endif

class Document {

private:

	std::vector<Mark> marks;
	std::vector<BookMark> bookmarks;
	std::vector<Link> links;
	sqlite3* db = nullptr;
	std::vector<TocNode*> top_level_toc_nodes;
	std::vector<std::wstring> flat_toc_names;
	std::vector<int> flat_toc_pages;


	// number of pages in the document
	std::optional<int> cached_num_pages = {};

	std::vector<std::pair<int, fz_stext_page*>> cached_stext_pages;
	std::vector<std::pair<int, fz_pixmap*>> cached_small_pixmaps;

	fz_context* context = nullptr;
	std::wstring file_name;
	std::unordered_map<int, fz_link*> cached_page_links;
	QStandardItemModel* cached_toc_model = nullptr;

	std::vector<float> accum_page_heights;
	std::vector<float> page_heights;
	std::vector<float> page_widths;
	std::mutex page_dims_mutex;

	// These are a heuristic index of all figures and references in the document
	// The reason that we use a hashmap for reference_indices and a vector for figures is that
	// the reference we are looking for is usually the last reference with that name, but this is not
	// necessarily true for the figures.
	std::vector<IndexedData> figure_indices;
	std::map<std::wstring, IndexedData> reference_indices;
	std::map<std::wstring, IndexedData> equation_indices;

	std::mutex figure_indices_mutex;
	std::optional<std::thread> figure_indexing_thread = {};
	bool is_figure_indexing_required = true;
	bool is_indexing = false;

	QDateTime last_update_time;

	// we do some of the document processing in a background thread (for example indexing all the
	// figures/indices and computing page heights. we use this pointer to notify the main thread when
	// processing is complete.
	bool* invalid_flag_pointer = nullptr;

	int get_mark_index(char symbol);
	fz_outline* get_toc_outline();

	// load marks, bookmarks, links, etc.
	void load_document_metadata_from_db();

	// convetr the fz_outline structure to our own TocNode structure
	void create_toc_tree(std::vector<TocNode*>& toc);

	Document(fz_context* context, std::wstring file_name, sqlite3* db);
public:
	fz_document* doc = nullptr;

	void add_bookmark(const std::wstring& desc, float y_offset);
	void count_chapter_pages(std::vector<int> &page_counts);
	void convert_toc_tree(fz_outline* root, std::vector<TocNode*>& output);
	void count_chapter_pages_accum(std::vector<int> &page_counts);
	bool get_is_indexing();
	fz_stext_page* get_stext_with_page_number(int page_number);
	void add_link(Link link, bool insert_into_database = true);
	std::wstring get_path();
	int find_closest_bookmark_index(float to_offset_y);
	std::optional<Link> find_closest_link(float to_offset_y, int* index = nullptr);
	bool update_link(Link new_link);
	void delete_closest_bookmark(float to_y_offset);
	void delete_closest_link(float to_offset_y);
	const std::vector<BookMark>& get_bookmarks() const;
	fz_link* get_page_links(int page_number);
	void add_mark(char symbol, float y_offset);
	bool remove_mark(char symbol);
	bool get_mark_location_if_exists(char symbol, float* y_offset);
	~Document();
	const std::vector<TocNode*>& get_toc();
	bool has_toc();
	const std::vector<std::wstring>& get_flat_toc_names();
	const std::vector<int>& get_flat_toc_pages();
	bool open(bool* invalid_flag);
	void reload();
	QDateTime get_last_edit_time();
	unsigned int get_milies_since_last_document_update_time();
	unsigned int get_milies_since_last_edit_time();
	float get_page_height(int page_index);
	fz_pixmap* get_small_pixmap(int page);
	float get_page_width(int page_index);
	float get_page_width_smart(int page_index, float* left_ratio, float* right_ratio, int* normal_page_width);
	float get_accum_page_height(int page_index);
	//const vector<float>& get_page_heights();
	//const vector<float>& get_page_widths();
	//const vector<float>& get_accum_page_heights();
	void get_visible_pages(float doc_y_range_begin, float doc_y_range_end, std::vector<int>& visible_pages);
	void load_page_dimensions();
	int num_pages();
	fz_rect get_page_absolute_rect(int page);
	void absolute_to_page_pos(float absolute_x, float absolute_y, float* doc_x, float* doc_y, int* doc_page);
	QStandardItemModel* get_toc_model();
	void page_pos_to_absolute_pos(int page, float page_x, float page_y, float* abs_x, float* abs_y);
	fz_rect page_rect_to_absolute_rect(int page, fz_rect page_rect);
	int get_offset_page_number(float y_offset);
	void index_figures(bool* invalid_flag);
	void stop_indexing();
	bool find_figure_with_string(std::wstring figure_name, int reference_page, int* page, float* y_offset);
	std::optional<IndexedData> find_reference_with_string(std::wstring reference_name);
	std::optional<IndexedData> find_equation_with_string(std::wstring equation_name);

	std::optional<std::wstring> get_text_at_position(std::vector<fz_stext_char*> flat_chars, float offset_x, float offset_y);
	std::optional<std::wstring> get_reference_text_at_position(std::vector<fz_stext_char*> flat_chars, float offset_x, float offset_y);
	std::optional<std::wstring> get_paper_name_at_position(std::vector<fz_stext_char*> flat_chars, float offset_x, float offset_y);
	std::optional<std::wstring> get_equation_text_at_position(std::vector<fz_stext_char*> flat_chars, float offset_x, float offset_y);
	//std::optional<std::pair<std::wstring, std::wstring>> get_all_text_objects_at_location(std::vector<fz_stext_char*> flat_chars, float offset_x, float offset_y);
	friend class DocumentManager;
};

class DocumentManager {
private:
	fz_context* mupdf_context = nullptr;
	sqlite3* database = nullptr;
	std::unordered_map<std::wstring, Document*> cached_documents;
public:

	DocumentManager(fz_context* mupdf_context, sqlite3* database);

	Document* get_document(std::filesystem::path path);
	const std::unordered_map<std::wstring, Document*>& get_cached_documents();;
	void delete_global_mark(char symbol);
};

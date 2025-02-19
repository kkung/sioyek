#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <optional>
#include <utility>
#include <memory>
#include <filesystem>

#include <qapplication.h>
#include <qpushbutton.h>
#include <qopenglwidget.h>
#include <qopenglextrafunctions.h>
#include <qopenglfunctions.h>
#include <qopengl.h>
#include <qwindow.h>
#include <qkeyevent.h>
#include <qlineedit.h>
#include <qtreeview.h>
#include <qsortfilterproxymodel.h>
#include <qabstractitemmodel.h>
#include <qopenglshaderprogram.h>
#include <qtimer.h>
#include <qdatetime.h>
#include <qstackedwidget.h>
#include <qboxlayout.h>
#include <qlistview.h>
#include <qstringlistmodel.h>
#include <qlabel.h>
#include <qtextedit.h>
#include <qfilesystemwatcher.h>
#include <qdesktopwidget.h>

#include "document_view.h"



struct OpenGLSharedResources {
	GLuint vertex_buffer_object;
	GLuint uv_buffer_object;
	GLuint rendered_program;
	GLuint rendered_dark_program;
	GLuint unrendered_program;
	GLuint highlight_program;
	GLuint vertical_line_program;
	GLuint vertical_line_dark_program;

	GLint dark_mode_contrast_uniform_location;
	GLint highlight_color_uniform_location;
	GLint line_color_uniform_location;
	GLint line_time_uniform_location;
	GLint line_freq_uniform_location;

	bool is_initialized;
};


class PdfViewOpenGLWidget : public QOpenGLWidget, protected QOpenGLExtraFunctions {
private:
	static OpenGLSharedResources shared_gl_objects;

	bool is_opengl_initialized = false;
	GLuint vertex_array_object;
	DocumentView* document_view = nullptr;
	PdfRenderer* pdf_renderer = nullptr;
	ConfigManager* config_manager = nullptr;
	std::vector<SearchResult> search_results;
	int current_search_result_index = -1;
	std::mutex search_results_mutex;
	bool is_search_cancelled = true;
	bool is_searching;
	bool should_highlight_links = false;
	bool is_dark_mode = false;
	float percent_done = 0.0f;

	//float vertical_line_location;
	bool should_draw_vertical_line = false;
	QDateTime creation_time;

	GLuint LoadShaders(std::filesystem::path vertex_file_path_, std::filesystem::path fragment_file_path_);
protected:

	void initializeGL() override;
	void resizeGL(int w, int h) override;
	void render_highlight_window(GLuint program, fz_rect window_rect);
	void render_line_window(GLuint program, float vertical_pos);
	void render_highlight_absolute(GLuint program, fz_rect absolute_document_rect);
	void render_highlight_document(GLuint program, int page, fz_rect doc_rect);
	void paintGL() override;
	void render();

public:

#ifndef NDEBUG
	// properties for visualizing selected blocks, used only for debug
	std::optional<int> last_selected_block_page = {};
	std::optional<fz_rect> last_selected_block = {};
#endif

	std::vector<fz_rect> selected_character_rects;

	PdfViewOpenGLWidget(DocumentView* document_view, PdfRenderer* pdf_renderer, ConfigManager* config_manager, QWidget* parent = nullptr);
	~PdfViewOpenGLWidget();

	//void set_vertical_line_pos(float pos);
	//float get_vertical_line_pos();
	void set_should_draw_vertical_line(bool val);
	bool get_should_draw_vertical_line();
	void handle_escape();
	void toggle_highlight_links();
	int get_num_search_results();
	int get_current_search_result_index();
	bool valid_document();
	void goto_search_result(int offset);
	void render_page(int page_number);
	bool get_is_searching(float* prog);
	void search_text(const std::wstring& text, std::optional<std::pair<int, int>> range = {});
	void set_dark_mode(bool mode);
};

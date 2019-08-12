#pragma once
#include <string>
#include <thread>

#include <Windows.h>

#include <d3d9.h>
#include <d3dx9.h>
#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")

namespace forceinline {
	class dx_overlay {
	private:
		struct wnd_rect_t : public RECT {
			int width( ) { return right - left; }
			int height( ) { return bottom - top; }
		};

	public:
		dx_overlay( ) { }
		dx_overlay( std::wstring_view target_class, std::wstring_view target_window, bool not_topmost = false );
		~dx_overlay( );

		void begin_rendering( );
		void end_rendering( );

		IDirect3DDevice9* get_device( );
		int get_fps( );
		HWND get_overlay_wnd( );

		bool is_initialized( );

	private:
		void create_overlay( std::wstring_view target_class, std::wstring_view target_window );
		void init_dx9( );

		int m_fps = 0;
		bool m_initialized = false, m_not_topmost = false;

		HWND m_overlay_wnd = nullptr, m_target_wnd = nullptr;
		wnd_rect_t m_overlay_wnd_size, m_target_wnd_size;

		IDirect3D9* m_d3d = nullptr;
		IDirect3DDevice9* m_device = nullptr;

		static LRESULT CALLBACK m_wnd_proc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );
	};
}
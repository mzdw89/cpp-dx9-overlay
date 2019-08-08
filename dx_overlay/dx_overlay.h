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
	public:
		dx_overlay( ) { }
		dx_overlay( HINSTANCE instance, std::wstring_view parent_class, std::wstring_view parent_window );
		~dx_overlay( );

		void begin_rendering( );
		void end_rendering( );

		IDirect3DDevice9* get_device( );
		int get_fps( );
		HWND get_overlay_wnd( );

		bool is_initialized( );

	private:
		void create_overlay( HINSTANCE instance, std::wstring_view parent_class, std::wstring_view parent_window );
		void init_dx9( );

		int m_fps = 0;
		bool m_initialized = false;

		HWND m_overlay_wnd = nullptr;
		
		IDirect3D9* m_d3d = nullptr;
		IDirect3DDevice9* m_device = nullptr;
	
		static LRESULT CALLBACK m_wnd_proc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );
	};
}

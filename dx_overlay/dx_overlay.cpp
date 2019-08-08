#include "dx_overlay.h"

#include <iostream>
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")

namespace forceinline {
	dx_overlay::dx_overlay( HINSTANCE instance, std::wstring_view parent_class, std::wstring_view parent_window ) {
		try {
			if ( !instance )
				throw std::invalid_argument( "dx_overlay::dx_overlay: instance is invalid" );

			if ( parent_window.empty( ) && parent_class.empty( ) )
				throw std::invalid_argument( "dx_overlay::dx_overlay: parent_class and parent_window empty" );

			if ( !FindWindowW( parent_class.empty( ) ? NULL : parent_class.data( ), parent_window.empty( ) ? NULL : parent_window.data( ) ) ) {
				std::string parent_class_mb( parent_class.begin( ), parent_class.end( ) );
				std::string parent_window_mb( parent_window.begin( ), parent_window.end( ) );

				throw std::invalid_argument( "dx_overlay::dx_overlay: parent window \"" + parent_window_mb + "\" with parent class \"" + parent_class_mb + "\" could not be found" );
			}

			create_overlay( instance, parent_class, parent_window );
			init_dx9( );
		} catch ( const std::exception& e ) {
			std::cerr << e.what( ) << std::endl;
		}
	}

	dx_overlay::~dx_overlay( ) {
		if ( m_overlay_wnd )
			DestroyWindow( m_overlay_wnd );

		if ( m_d3d )
			m_d3d->Release( );

		if ( m_device )
			m_device->Release( );
	}

	void dx_overlay::begin_rendering( ) {
		m_device->Clear( NULL, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB( 0, 0, 0, 0 ), 1.f, 0 );
		m_device->BeginScene( );
	}

	void dx_overlay::end_rendering( ) {
		static int fps = 0;
		static float last_tick_count = 0.f;

		fps++;
		float cur_tick_count = clock( ) * 0.001f;
		if ( cur_tick_count - last_tick_count >= 1.f ) {
			last_tick_count = cur_tick_count;
			m_fps = fps;
			fps = 0;
		}

		m_device->EndScene( );
		m_device->Present( NULL, NULL, NULL, NULL );
	}

	IDirect3DDevice9* dx_overlay::get_device( ) {
		return m_device;
	}

	int dx_overlay::get_fps( ) {
		return m_fps;
	}

	HWND dx_overlay::get_overlay_wnd( ) {
		return m_overlay_wnd;
	}

	bool dx_overlay::is_initialized( ) {
		return m_initialized;
	}

	void dx_overlay::create_overlay( HINSTANCE instance, std::wstring_view parent_class, std::wstring_view parent_window ) {
		WNDCLASSEX wc;
		wc.cbSize = sizeof( wc );

		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = m_wnd_proc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = instance;
		wc.hIcon = NULL;
		wc.hCursor = LoadCursor( NULL, IDC_ARROW );
		wc.hbrBackground = NULL;
		wc.lpszMenuName = "";
		wc.lpszClassName = "forceinline::dx_overlay";
		wc.hIconSm = NULL;

		if ( !RegisterClassExA( &wc ) )
			throw std::exception( "dx_overlay::create_overlay: failed to register wndclassex" );

		HWND parent_wnd = FindWindowW( parent_class.data( ), parent_window.data( ) );
		
		RECT parent_wnd_size;
		GetClientRect( parent_wnd, &parent_wnd_size );

		m_overlay_wnd = CreateWindowExA( NULL, "forceinline::dx_overlay", "forceinline::dx_overlay", WS_EX_TOPMOST | WS_POPUP,
										 parent_wnd_size.left, parent_wnd_size.top, parent_wnd_size.right - parent_wnd_size.left, parent_wnd_size.bottom - parent_wnd_size.top, parent_wnd, NULL, instance, NULL );

		if ( !m_overlay_wnd )
			throw std::exception( "dx_overlay::create_overlay: failed to create overlay window" );

		DWORD cur_style = GetWindowLong( m_overlay_wnd, GWL_EXSTYLE );	
		SetWindowLong( m_overlay_wnd, GWL_EXSTYLE, cur_style | WS_EX_LAYERED | WS_EX_TRANSPARENT );	
		SetLayeredWindowAttributes( m_overlay_wnd, RGB( 0, 0, 0 ), 255, LWA_COLORKEY | LWA_ALPHA );

		ShowWindow( m_overlay_wnd, SW_SHOW );
	}

	void dx_overlay::init_dx9( ) {
		m_d3d = Direct3DCreate9( D3D_SDK_VERSION );

		if ( !m_d3d )
			throw std::exception( "dx_overlay::init_dx9: failed to create dx3d9 object" );

		D3DPRESENT_PARAMETERS d3d_pp;
		ZeroMemory( &d3d_pp, sizeof( d3d_pp ) );

		d3d_pp.Windowed = true;
		d3d_pp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		d3d_pp.BackBufferFormat = D3DFMT_UNKNOWN;
		d3d_pp.hDeviceWindow = m_overlay_wnd;
		d3d_pp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
		
		if ( FAILED( m_d3d->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_overlay_wnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3d_pp, &m_device ) ) ) {
			m_d3d->Release( );
			throw std::exception( "dx_overlay::init_dx9: failed to create device" );
		}

		m_initialized = true;
	}

	LRESULT CALLBACK dx_overlay::m_wnd_proc( HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam ) {
		MARGINS m{ 0, 0, 0, 1 };

		switch ( msg ) {
			case WM_CREATE:
				DwmExtendFrameIntoClientArea( wnd, &m );
				SetWindowPos( wnd, NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED );
				return TRUE;
			case WM_NCCALCSIZE:
				if ( wparam == TRUE ) {
					SetWindowLong( wnd, NULL, NULL );
					return TRUE;
				}
				return FALSE;
			default:
				return DefWindowProc( wnd, msg, wparam, lparam );
		}
	}
}

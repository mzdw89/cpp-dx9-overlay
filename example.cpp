#include "dx_overlay/dx_overlay.h"
#include <thread>

int main( ) {
	try {
		forceinline::dx_overlay overlay( L"CoDBlackOps", L"Call of Duty®: Black Ops II - Multiplayer", true /*Read comments regarding this*/ );

		MSG m;
		ZeroMemory( &m, sizeof( m ) );

		forceinline::dx_renderer renderer = overlay.create_renderer( );

		while ( m.message != WM_QUIT ) {
			if ( PeekMessage( &m, overlay.get_overlay_wnd( ), NULL, NULL, PM_REMOVE ) ) {
				TranslateMessage( &m );
				DispatchMessage( &m );
			} 

			renderer.begin_rendering( );
			
			renderer.draw_text( std::to_string( renderer.get_fps( ) ), 2, 2, 0xFFFFFFFF, false );
			
			renderer.end_rendering( );
			
			std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );
		}
	}
}

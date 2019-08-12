#include "dx_overlay/dx_overlay.h"
#include <thread>

int main( ) {
	forceinline::dx_overlay overlay( proc_instance, L"CoDBlackOps", L"Call of Duty®: Black Ops II - Multiplayer" );

	if ( !overlay.is_initialized( ) )
		return 0;

	MSG m;
	ZeroMemory( &m, sizeof( m ) );

	while ( m.message != WM_QUIT ) {
		if ( PeekMessage( &m, overlay.get_overlay_wnd( ), NULL, NULL, PM_REMOVE ) ) {
			TranslateMessage( &m );
			DispatchMessage( &m );
		} else {
			overlay.begin_rendering( );

			/*render here*/

			overlay.end_rendering( );
		}

		std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );
	}
}

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>

int main ( int argc, char const *argv[] )
{
    FILE * CONST pF  = fopen ( "src/main.c", "rb" );
    fseek ( pF, 0, SEEK_END );
    CONST UINT nSize = ftell ( pF );
    fseek ( pF, 0, SEEK_SET );
    BYTE * CONST pFileBuf = ( BYTE* ) malloc ( nSize );

    UINT nSizeReaded = 0;
    while ( nSizeReaded < nSize ) {
        nSizeReaded += fread ( pFileBuf + nSizeReaded, 1, nSize - nSizeReaded, pF );
    }

    fclose ( pF );


    FILE * CONST pFOut  = fopen ( "src/main.x", "wb" );
    UINT nLine = 0;
    UINT i0 = 0;
    VOID _newLine ( ) {
        fprintf ( pFOut, "#line %d\n", ++nLine );
    }
    _newLine ( );
    for ( UINT i = 0; i < nSize; ++i ) {
        if ( pFileBuf [ i ] == '\n' ) {
            if ( i > i0 ) {
                fprintf ( pFOut, "%.*s\n", i - i0, pFileBuf + i0 );
            }
            _newLine ( );
            i0 = i+1;
        }
    }

    fclose ( pFOut );

    return 0;
}

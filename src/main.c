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

    UINT nSizeWrited = 0;
    while ( nSizeWrited < nSize ) {
        nSizeWrited += fwrite ( pFileBuf + nSizeWrited, 1, nSize - nSizeWrited, pFOut );
    }

    fclose ( pFOut );

    return 0;
}

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>

#define IS_SPACE(_c) ((_c)==' '||(_c)=='\t')
#define IS_NUM_BIN(_c) ((_c)=='0'||(_c)=='1')
#define IS_NUM_OCT(_c) ((_c)>='0'&&(_c)<='7')
#define IS_NUM_DEC(_c) ((_c)>='0'&&(_c)<='9')
#define IS_NUM_HEX(_c) (IS_NUM_DEC(_c)||((_c)>='A'&&(_c)<='F')||((_c)>='a'&&(_c)<='f'))
#define IS_ALPHA_UPPER(_c) ((_c)>='A'&&(_c)<='Z')
#define IS_ALPHA_LOWER(_c) ((_c)>='a'&&(_c)<='z')

#define IS_IDENT_START(_c) ((_c)=='_'||IS_ALPHA_UPPER(_c)||IS_ALPHA_LOWER(_c))
#define IS_IDENT(_c) (IS_IDENT_START(_c)||IS_NUM_DEC(_c))


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
    UINT i1 = 0;
    VOID _newLine ( ) {
        fprintf ( pFOut, "#line %d\n", ++nLine );
    }
    _newLine ( );
    for ( UINT i = 0; i < nSize; ) {
        if ( pFileBuf [ i ] == '\n' ) {
            if ( i > i0 ) {
                fprintf ( pFOut, "%.*s\n", i - i0, pFileBuf + i0 );
            }
            _newLine ( );
            i0 = i+1;
            ++i;
        } else
        if ( IS_IDENT_START ( pFileBuf [ i ] ) ) {
            if ( i > i1 ) {
                fprintf ( pFOut, "OTHERS(\"%.*s\")\n", i - i1, pFileBuf + i1 );
            }
            CONST UINT _i = i;
            while ( IS_IDENT ( pFileBuf [ i ] ) ) {
                ++i;
            }
            i1 = i;
            fprintf ( pFOut, "IDENT(\"%.*s\")\n", i - _i, pFileBuf + _i );
        } else
        if ( IS_SPACE ( pFileBuf [ i ] ) ) {
            if ( i > i1 ) {
                fprintf ( pFOut, "OTHERS(\"%.*s\")\n", i - i1, pFileBuf + i1 );
            }
            CONST UINT _i = i;
            while ( IS_SPACE ( pFileBuf [ i ] ) ) {
                ++i;
            }
            i1 = i;
            fprintf ( pFOut, "SPACES(%d)\n", i - _i );
        } else
        if ( IS_NUM_DEC ( pFileBuf [ i ] ) ) {
            if ( i > i1 ) {
                fprintf ( pFOut, "OTHERS(\"%.*s\")\n", i - i1, pFileBuf + i1 );
            }
            CONST UINT _i = i;
            while ( IS_NUM_DEC ( pFileBuf [ i ] ) ) {
                ++i;
            }
            i1 = i;
            fprintf ( pFOut, "NUMS(\"%.*s\")\n", i - _i, pFileBuf + _i );
        } else {
            ++i;
        }
    }

    if ( nSize > i1 ) {
        fprintf ( pFOut, "OTHERS(\"%.*s\")\n", nSize - i1, pFileBuf + i1 );
    }
    if ( nSize > i0 ) {
        fprintf ( pFOut, "%.*s\n", nSize - i0, pFileBuf + i0 );
    }

    fclose ( pFOut );

    return 0;
}

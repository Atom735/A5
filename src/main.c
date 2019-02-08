#include <windows.h>
#include <stdlib.h>
#include <stdio.h>

#define IS_SPACE(_c) ((_c)==' '||(_c)=='\t')
#define IS_SIGN(_c) ((_c)=='+'||(_c)=='-')
#define IS_NUM_BIN(_c) ((_c)=='0'||(_c)=='1')
#define IS_NUM_OCT(_c) ((_c)>='0'&&(_c)<='7')
#define IS_NUM_DEC(_c) ((_c)>='0'&&(_c)<='9')
#define IS_NUM_HEX(_c) (IS_NUM_DEC(_c)||((_c)>='A'&&(_c)<='F')||((_c)>='a'&&(_c)<='f'))
#define IS_ALPHA_UPPER(_c) ((_c)>='A'&&(_c)<='Z')
#define IS_ALPHA_LOWER(_c) ((_c)>='a'&&(_c)<='z')

#define IS_IDENT_START(_c) ((_c)=='_'||IS_ALPHA_UPPER(_c)||IS_ALPHA_LOWER(_c))
#define IS_IDENT(_c) (IS_IDENT_START(_c)||IS_NUM_DEC(_c))
#define IS_PUNCTUATOR(_i) ((_i)=='['||(_i)==']'||(_i)=='('||(_i)==')'||(_i)=='{'||(_i)=='}'||(_i)=='.'||(_i)=='&'||(_i)=='*'||(_i)=='+'||(_i)=='-'||(_i)=='~'||(_i)=='!'||(_i)=='/'||(_i)=='%'||(_i)=='<'||(_i)=='>'||(_i)=='='||(_i)=='^'||(_i)=='|'||(_i)=='?'||(_i)==':'||(_i)==';'||(_i)=='='||(_i)==','||(_i)=='#')

typedef struct tag_S_TOKEN {
    UINT iType; /* Тип токена */
    UINT nPos;  /* Позиция в файле */
    UINT nSize; /* Длина токена */
    struct tag_S_TOKEN * pNext;
} S_TOKEN;

S_TOKEN * token_push ( S_TOKEN * CONST pLast, CONST UINT iType, CONST UINT nPos, CONST UINT nSize ) {
    S_TOKEN * CONST pNext = ( S_TOKEN* ) malloc ( sizeof ( S_TOKEN ) );
    pLast -> pNext = pNext;
    pNext -> iType = iType;
    pNext -> nPos = nPos;
    pNext -> nSize = nSize;
    pNext -> pNext = NULL;
    return pNext;
}

enum {
    TN_NULL = 0,
    TN_HEAD,
    TN_LINE_FEED,
    TN_WHITE_SPACE,
    TN_CONST_CHAR,
    TN_CONST_STRING,
    TN_PUNCTUATOR,
    TN_PP_NUMBER,
    TN_IDENTIFIER,
    TN_COMMENT_BLOCK,
    TN_COMMENT_LINE,
};

int main ( int argc, char const *argv[] )
{
    FILE * CONST pF  = fopen ( "src/main.c""""", "rb" );
    fseek ( pF, 0, SEEK_END );
    CONST UINT nSize = ftell ( pF );
    fseek ( pF, 0, SEEK_SET );
    BYTE * CONST pFileBuf = ( BYTE* ) malloc ( nSize );

    UINT nSizeReaded = 0;
    while ( nSizeReaded < nSize ) {
        nSizeReaded += fread ( pFileBuf + nSizeReaded, 1, nSize - nSizeReaded, pF );
    }

    fclose ( pF );

    S_TOKEN pHead = {
        .iType = TN_HEAD,
    };
    S_TOKEN *pLast = &pHead;

    UINT nLine = 1;
    UINT i = 0;
    while ( i < nSize ) {
        if ( pFileBuf [ i ] == '/' ) {
            if ( pFileBuf [ i + 1 ] == '/' ) {
                CONST UINT nPos = i;
                i += 2;
                while ( pFileBuf [ i ] != '\n' || pFileBuf [ i - 1 ] == '\\' ) {
                    ++i;
                }
                pLast = token_push ( pLast, TN_COMMENT_LINE, nPos, i - nPos );
                continue;
            }
            if ( pFileBuf [ i + 1 ] == '*' ) {
                CONST UINT nPos = i;
                i += 3;
                while ( pFileBuf [ i ] != '/' || pFileBuf [ i - 1 ] != '*' ) {
                    ++i;
                }
                ++i;
                pLast = token_push ( pLast, TN_COMMENT_BLOCK, nPos, i - nPos );
                continue;
            }
        }
        if ( IS_NUM_DEC ( pFileBuf [ i ] ) || ( pFileBuf [ i ] == '.' && IS_NUM_DEC ( pFileBuf [ i + 1 ] ) ) ) {
            CONST UINT nPos = i;
            ++i;
            while ( IS_NUM_DEC ( pFileBuf [ i ] ) || pFileBuf [ i ] == '.' || IS_IDENT_START ( pFileBuf [ i ] ) ) {
                switch ( pFileBuf [ i ] ) {
                    case 'e': case 'E':
                    case 'p': case 'P':
                    if ( IS_SIGN ( pFileBuf [ i + 1 ] ) ) {
                        ++i;
                    } else {
                        ++i;
                        pLast = token_push ( pLast, TN_PP_NUMBER, nPos, i - nPos );
                        continue;
                    }
                    break;
                }
                ++i;
            }
            pLast = token_push ( pLast, TN_PP_NUMBER, nPos, i - nPos );
            continue;
        }
        if ( pFileBuf [ i ] == '\n' ) {
            pLast = token_push ( pLast, TN_LINE_FEED, i, nLine );
            ++nLine;
            ++i;
            continue;
        }
        if ( pFileBuf [ i ] == '\'' ) {
            CONST UINT nPos = i;
            ++i;
            while ( pFileBuf [ i ] != '\'' || pFileBuf [ i - 1 ] == '\\' ) {
                if ( pFileBuf [ i - 1 ] == '\\' ) {
                    UINT k = 2;
                    while ( pFileBuf [ i - k ] == '\\' ) {
                        ++k;
                    }
                    if ( k % 2 == 1 ) {
                        break;
                    }
                }
                ++i;
            }
            ++i;
            pLast = token_push ( pLast, TN_CONST_CHAR, nPos, i - nPos );
            continue;
        }
        if ( pFileBuf [ i ] == '\"' ) {
            CONST UINT nPos = i;
            ++i;
            while ( pFileBuf [ i ] != '\"' || pFileBuf [ i - 1 ] == '\\' ) {
                ++i;
            }
            ++i;
            pLast = token_push ( pLast, TN_CONST_STRING, nPos, i - nPos );
            continue;
        }
        if ( IS_SPACE ( pFileBuf [ i ] ) ) {
            CONST UINT nPos = i;
            ++i;
            while ( IS_SPACE ( pFileBuf [ i ] ) ) {
                ++i;
            }
            pLast = token_push ( pLast, TN_WHITE_SPACE, nPos, i - nPos );
            continue;
        }
        if ( IS_PUNCTUATOR ( pFileBuf [ i ] ) ) {
            pLast = token_push ( pLast, TN_PUNCTUATOR, i, 1 );
            ++i;
            continue;
        }
        if ( IS_IDENT_START ( pFileBuf [ i ] ) ) {
            CONST UINT nPos = i;
            ++i;
            while ( IS_IDENT ( pFileBuf [ i ] ) ) {
                ++i;
            }
            pLast = token_push ( pLast, TN_IDENTIFIER, nPos, i - nPos );
            continue;
        }
        ++i;
    }


    FILE * CONST pFOut  = fopen ( "src/main.x", "wb" );

    pLast = &pHead;

    while ( pLast -> pNext != NULL ) {
        pLast = pLast -> pNext;
        switch ( pLast -> iType ) {
        case TN_NULL: fprintf ( pFOut, "TN_NULL();\n" ); continue;
        case TN_HEAD: fprintf ( pFOut, "TN_HEAD();\n" ); continue;
        case TN_LINE_FEED: fprintf ( pFOut, "#line %d\n", pLast -> nSize ); continue;
        case TN_WHITE_SPACE: fprintf ( pFOut, "TN_WHITE_SPACE(%d);\n", pLast -> nSize ); continue;
        case TN_CONST_CHAR: fprintf ( pFOut, "TN_CONST_CHAR(%.*s);\n", pLast -> nSize, pFileBuf + pLast -> nPos ); continue;
        case TN_CONST_STRING: fprintf ( pFOut, "TN_CONST_STRING(%.*s);\n", pLast -> nSize, pFileBuf + pLast -> nPos ); continue;
        case TN_PUNCTUATOR: fprintf ( pFOut, "TN_PUNCTUATOR(\"%.*s\");\n", pLast -> nSize, pFileBuf + pLast -> nPos ); continue;
        case TN_PP_NUMBER: fprintf ( pFOut, "TN_PP_NUMBER(%.*s);\n", pLast -> nSize, pFileBuf + pLast -> nPos ); continue;
        case TN_IDENTIFIER: fprintf ( pFOut, "TN_IDENTIFIER(\"%.*s\");\n", pLast -> nSize, pFileBuf + pLast -> nPos ); continue;


        case TN_COMMENT_LINE: fprintf ( pFOut, "#if 0\n#LINE: %.*s\n#endif\n", pLast -> nSize, pFileBuf + pLast -> nPos ); continue;
        case TN_COMMENT_BLOCK: fprintf ( pFOut, "#if 0\n#BLOCK: %.*s\n#endif\n", pLast -> nSize, pFileBuf + pLast -> nPos ); continue;
        }
    }

    fclose ( pFOut );

    return 0;
}

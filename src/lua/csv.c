// build@ gcc -g -shared -I/home/sdonovan/lua/include -o csv.so csv.c
/* luaCSV
 * Steve Donovan, August 2009
 * MIT/X11 Licence.
 * (1) This is my dog
 * (2) My dog did not eat your homework.

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// includes for Lua
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#define BUFSZ 10240
#define MAXFIELD 512
#define QUOTE '"'
#define EQ(s1,s2) (strcmp((s1),(s2))==0)

#define lua_objlen(L,i)     lua_rawlen(L, (i))
#define luaL_putchar(B,c)   luaL_addchar(B,c)

#define CSV_META_READER "*CSV_RD"
#define CSV_META_WRITER "*CSV_WT"
#define CSV_META_ARRAY "*CSV_ARR"

static char buffer[BUFSZ];

typedef int boolean;
enum {
    FALSE, TRUE
};

typedef unsigned char uchar;

enum {
    CHECK = 1,
    NUMBER = 2
};

typedef struct {
    FILE* file;
    int nfield;
    uchar* cols;
    int tbl;
    char delim;
    int line;
} CsvData;

static int convert_csv_line( lua_State* L, int idx, char delim ) {
    luaL_Buffer b;
    const char* str;
    int i, n = lua_objlen( L, idx );
    luaL_buffinit( L, &b );

    for ( i = 1; i <= n; i++ ) {
        lua_rawgeti( L, idx, i );
        str = lua_tostring( L, -1 );

        if ( strchr( str, delim ) ||
                strchr( str, QUOTE ) ) { // wd strcspn be much faster?
            luaL_putchar( &b, QUOTE );

            for ( ; *str; ++str ) {
                luaL_putchar( &b, *str );

                if ( *str == QUOTE ) {
                    luaL_putchar( &b, QUOTE );
                }
            }

            luaL_putchar( &b, QUOTE );
        }
        else {
            luaL_addstring( &b, str );
        }

        if ( i != n ) {
            luaL_putchar( &b, delim );
        }
    }

    luaL_pushresult( &b );
    return 1;
}

/* used for hunting for next delimiter, usually ',' or QUOTE. */
static const char* copy_field( char* field, const char* p, char delim,
                               boolean escape ) {
    while ( *p && *p != delim ) {
        *field++ = *p++;

        /* double-quotes are escaped by doubling-up */
        if ( *p == QUOTE && *( p + 1 ) == QUOTE ) {
            *field++ = QUOTE;
            p += 2;  /* so we skip the next quote */
        }
    }

    *field = '\0';
    return p;
}

static void trim_line_end( char* p ) {
    p += strlen( p ) - 1;

    while ( *p == '\n' || *p == '\r' ) {
        --p;
    }

    *( p + 1 ) = '\0';
}

static int parse_csv_line( lua_State* L, const char* p, char delim,
                           uchar* cols ) {
    boolean eoln, quoted;
    int k = 1;
    char field[MAXFIELD];

    while ( 1 ) { /* for all fields */
        quoted = *p == QUOTE;

        if ( quoted ) {
            ++p;
        }

        p = copy_field( field, p, ( char )( quoted ? QUOTE : delim ), quoted );
        eoln = *p == '\0';

        if ( eoln ) {
            if ( quoted ) {
                return 0; /* ERROR! */
            }
            else {
                trim_line_end( field );
            }
        }
        else {
            p++;

            if ( quoted ) {
                ++p;

                if ( *p == '\0' || *( p + 1 ) == '\0' ) {
                    trim_line_end( field );
                    eoln = TRUE;
                }
            }
        }

        /* field is extracted
        auto-conversion: originally 'cols' is CHECK, so we experimentally try to convert
        the fields. If it fails, cool, mark it as not a number. Otherwise, it is a number,and
        mark the column with the extra field NUMBER.  A conversion error in this state
        is an error
        */
        if ( cols && ( cols[k] & CHECK ) ) {
            char* endp;
            double val;
            val = strtod( field, &endp );

            /* we are strict here; no trailing space after a number! */
            if ( endp == field || *endp ) {
                if ( cols[k] & NUMBER ) {
                    if ( *field != 0 ) {
                        fprintf( stderr, "bad field '%s'\n", field );
                        return -k;
                    }
                    else { /* empty numerical fields are considered zero */
                        val = 0.0;
                    }
                }
                else {
                    cols[k] = 0;  /* we were checking, and this is not a numerical field */
                    lua_pushstring( L, field );
                    goto table_set;
                }
            }
            else {
                cols[k] |= NUMBER;
            }

            lua_pushnumber( L, val );
        }
        else {   /* string field */
            lua_pushstring( L, field );
        }

table_set:
        lua_rawseti( L, -2, k );
        ++k;

        /* check if we've run out of line... */
        if ( eoln ) {
            break;
        }
    } /* fields */

    return k - 1;
}

static int parse_csv_file( lua_State* L, FILE* in, char delim, uchar* cols ) {
    if ( fgets( buffer, BUFSZ, in ) != NULL ) {
        return parse_csv_line( L, buffer, delim, cols );
    }
    else {
        return 0;
    }
}

static boolean optboolean( lua_State* L, int idx, boolean def ) {
    if ( lua_isnoneornil( L, idx ) ) {
        return def;
    }
    else {
        return lua_toboolean( L, idx );
    }
}

static CsvData* new_data( lua_State* L, const char* metaname, FILE* file,
                          char delim ) {
    CsvData* data = ( CsvData* )lua_newuserdata( L, sizeof( CsvData ) );
    data->file = file;
    data->nfield = 0;
    data->cols = NULL;
    data->tbl = 0;
    data->delim = delim;
    data->line = 1;
    luaL_newmetatable( L, metaname );
    lua_setmetatable( L, -2 );
    return data;
}

static int l_writer( lua_State* L ) {
    CsvData* data;
    const char* file = luaL_optstring( L, 1, "stdout" );
    char delim = *luaL_optstring( L, 2, "," );
    FILE* out = EQ( file, "stdout" ) ? stdout : fopen( file, "w" );

    if ( out == NULL ) {
        lua_pushnil( L );
        lua_pushstring( L, "cannot open file" );
        return 2;
    }

    data = new_data( L, CSV_META_WRITER, out, delim );
    return 1;
}

static int l_reader( lua_State* L ) {
    CsvData* data;
    const char* file = luaL_optstring( L, 1, "stdin" );
    boolean headers = optboolean( L, 2, TRUE );
    boolean try_convert = optboolean( L, 3, TRUE );
    char delim = *luaL_optstring( L, 4, "," );
    FILE* in = EQ( file, "stdin" ) ? stdin : fopen( file, "r" );

    if ( in == NULL ) {
        lua_pushnil( L );
        lua_pushstring( L, "cannot open file" );
        return 2;
    }

    data = new_data( L, CSV_META_READER, in, delim );

    if ( headers ) {
        int ncol;
        lua_newtable( L );
        ncol = parse_csv_file( L, in, delim, NULL );

        if ( ! ncol ) {
            lua_pop( L, 1 ); // get rid of userdata
            lua_pushnil( L );
            lua_pushstring( L, "empty file" );
            return 2;
        }

        data->nfield = ncol;
        luaL_newmetatable( L, CSV_META_ARRAY );
        lua_setmetatable( L, -2 );
    }

    if ( try_convert ) {
        int ncol = data->nfield ? data->nfield : MAXFIELD;
        data->cols = ( uchar* )malloc( ncol + 1 );
        memset( data->cols, CHECK, ncol + 1 );
    }

    return headers ? 2 : 1;
}

static void _close( CsvData* data ) {
    fclose( data->file );
    free( data->cols );
}

static int _readline( lua_State* L, CsvData* data, boolean push_error ) {
    int n = parse_csv_file( L, data->file, data->delim, data->cols );
    int nfield = data->nfield;

    if ( ! n ) {
        _close( data );
        return 0;
    }
    else if ( nfield && nfield != n ) {
        if ( push_error ) {
            lua_pushnil( L );
        }

        if ( n < 0 ) {
            lua_pushfstring( L, "line %d: conversion failed on field %d", data->line, -n );
        }
        else {
            lua_pushfstring( L, "line %d: wrong number of columns %d, expecting %d",
                             data->line, n, nfield );
        }

        if ( ! push_error ) {
            lua_error( L );
        }
    }
    else if ( ! nfield ) {
        data->nfield = n;
    }

    ++data->line;
    return 1;
}

static int l_read( lua_State* L ) {
    CsvData* data = ( CsvData* )lua_touserdata( L, 1 );
    lua_createtable( L, data->nfield, 0 );
    return _readline( L, data, TRUE );
}

static int l_write( lua_State* L ) {
    CsvData* data = ( CsvData* )lua_touserdata( L, 1 );
    convert_csv_line( L, 2, data->delim );
    fprintf( data->file, "%s\n", lua_tostring( L, -1 ) );
    return 0;
}

static int readr( lua_State* L ) {
    CsvData* data = ( CsvData* )lua_touserdata( L, lua_upvalueindex( 1 ) );
    // the iterator over all rows always returns the same table, for efficiency reasons.
    lua_rawgeti( L, LUA_REGISTRYINDEX, data->tbl );

    if ( lua_isnil( L, -1 ) ) {
        lua_createtable( L, data->nfield, 0 );
        luaL_newmetatable( L, CSV_META_ARRAY );
        lua_setmetatable( L, -2 );
        lua_pushvalue( L, -1 );
        data->tbl = luaL_ref( L, LUA_REGISTRYINDEX );
    }

    return _readline( L, data, FALSE );
}

static int l_rows( lua_State* L ) {
    lua_pushvalue( L, 1 );
    lua_pushcclosure( L, readr, 1 );
    return 1;
}


static int l_parse( lua_State* L ) {
    lua_newtable( L );

    if ( ! parse_csv_line( L, lua_tostring( L, 1 ), ',', NULL ) ) {
        return 0;
    }

    return 1;
}

static int l_convert( lua_State* L ) {
    return convert_csv_line( L, 1, *luaL_optstring( L, 2, "," ) );
}

static int l_close( lua_State* L ) {
    CsvData* data = ( CsvData* )lua_touserdata( L, 1 );
    _close( data );
    return 0;
}


static int l_copy( lua_State* L ) {
    // the table is at 1
    int i, n = lua_objlen( L, 1 );
    lua_createtable( L, n, 0 ); // our new table

    for ( i = 1; i <= n; i++ ) {
        lua_rawgeti( L, 1, i );
        lua_rawseti( L, -2, i );
    }

    return 1;
}

static int l_index( lua_State* L ) {
    // the table is at 1
    const char* s = lua_tostring( L, 2 );
    int i = -1;
    lua_pushnil( L );

    while ( lua_next( L, 1 ) != 0 ) {
        // key is at -2, value at -1
        if ( EQ( lua_tostring( L, -1 ), s ) ) {
            i = lua_tointeger( L, -2 );
            break;
        }

        lua_pop( L, 1 );
    }

    lua_pushnumber( L, i );
    return 1;
}

static const luaL_Reg csv_array[] = {
    {"copy", l_copy},
    {"index", l_index},
    {NULL, NULL},
};

static const luaL_Reg csv_reader[] = {
    {"read", l_read},
    {"rows", l_rows},
    {"close", l_close},
    {NULL, NULL},
};

static const luaL_Reg csv_writer[] = {
    {"write", l_write},
    {"close", l_close},
    {NULL, NULL},
};

static const luaL_Reg csv[] = {
    {"reader", l_reader},
    {"writer", l_writer},
    {"parse", l_parse},
    {"convert", l_convert},
    {NULL, NULL}
};

static void createmeta( lua_State* L, const char* name,
                        const luaL_Reg* methods ) {
    luaL_newmetatable( L, name );
    lua_pushvalue( L, -1 ); /* push metatable */
    lua_setfield( L, -2, "__index" ); /* metatable.__index = metatable */
    luaL_setfuncs( L, methods, 0 ); /* add file methods to new metatable */
    lua_pop( L, 1 ); /* pop new metatable */
}

#if defined(_WIN32) || defined(_WIN64)
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

LUALIB_API int luaopen_csv( lua_State* L ) {
    luaL_register( L, LUA_CSVLIBNAME, csv );
    createmeta( L, CSV_META_READER, csv_reader );
    createmeta( L, CSV_META_WRITER, csv_writer );
    createmeta( L, CSV_META_ARRAY, csv_array );
    return 1;
}

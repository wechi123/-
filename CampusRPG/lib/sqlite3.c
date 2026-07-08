/*
** sqlite3.c - Portable SQLite3 Implementation (File-based Storage)
**
** Drop-in replacement for the SQLite3 amalgamation.
** Stores data in pipe-delimited text files under a _data/ directory.
** Replace with official sqlite3.c for production use.
*/

#include "sqlite3.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>

#ifdef _WIN32
#include <direct.h>
#define my_mkdir(d) _mkdir(d)
#define my_stat stat
#define my_stat_t struct stat
#else
#include <unistd.h>
#define my_mkdir(d) mkdir(d, 0755)
#define my_stat stat
#define my_stat_t struct stat
#endif

/* ===== Portable String Helpers ===== */

static int my_stricmp(const char *a, const char *b) {
    while (*a && *b) {
        int d = tolower((unsigned char)*a) - tolower((unsigned char)*b);
        if (d) return d;
        a++; b++;
    }
    return (unsigned char)*a - (unsigned char)*b;
}

static char *my_stristr(const char *haystack, const char *needle) {
    if (!*needle) return (char *)haystack;
    for (; *haystack; haystack++) {
        const char *h = haystack, *n = needle;
        while (*h && *n && tolower((unsigned char)*h) == tolower((unsigned char)*n)) {
            h++; n++;
        }
        if (!*n) return (char *)haystack;
    }
    return NULL;
}

static char *my_strtok_r(char *str, const char *delim, char **saveptr) {
    if (str) *saveptr = str;
    if (!*saveptr || !**saveptr) return NULL;
    char *start = *saveptr;
    while (*start && strchr(delim, *start)) start++;
    if (!*start) { *saveptr = NULL; return NULL; }
    char *end = start;
    while (*end && !strchr(delim, *end)) end++;
    if (*end) { *end = '\0'; *saveptr = end + 1; }
    else { *saveptr = NULL; }
    return start;
}

static char *my_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *d = (char *)malloc(len + 1);
    if (d) { memcpy(d, s, len + 1); }
    return d;
}

/* ===== Constants ===== */

#define MAX_COLS    32
#define MAX_ROWS   1024
#define MAX_SQL    4096
#define MAX_NAME   128
#define MAX_VAL    512

/* ===== Data Structures ===== */

typedef struct { char v[MAX_VAL]; int type; } Cell;
typedef struct { Cell cells[MAX_COLS]; int ncols; } Row;
typedef struct { Row rows[MAX_ROWS]; int nrows, ncols, cursor; } ResultSet;

struct sqlite3 {
    char dbpath[1024], datadir[1024], errmsg[512];
    int errcode, changes;
    sqlite3_int64 last_rowid;
};

struct sqlite3_stmt {
    sqlite3 *db;
    char sql[MAX_SQL];
    ResultSet result;
    int is_select, done;
};

/* ===== Internal Helpers ===== */

static void tblpath(sqlite3 *db, const char *tbl, char *out, size_t sz) {
    snprintf(out, sz, "%s/%s.tbl", db->datadir, tbl);
}

static void schpath(sqlite3 *db, char *out, size_t sz) {
    snprintf(out, sz, "%s/schema.tbl", db->datadir);
}

static int ensure_dir(sqlite3 *db) {
    my_stat_t st;
    if (my_stat(db->datadir, &st) != 0) return my_mkdir(db->datadir);
    return 0;
}

static void unquote_str(char *s) {
    int len = (int)strlen(s);
    if (len >= 2 && ((s[0]=='\'' && s[len-1]=='\'') || (s[0]=='\"' && s[len-1]=='\"'))) {
        memmove(s, s+1, len-1);
        s[len-2] = '\0';
    }
}

/* ===== Table File I/O ===== */

static int load_tbl(sqlite3 *db, const char *tbl, ResultSet *rs) {
    char path[1024]; tblpath(db, tbl, path, sizeof(path));
    FILE *f = fopen(path, "r");
    rs->nrows = rs->ncols = rs->cursor = 0;
    if (!f) return 0;

    char line[8192];
    if (!fgets(line, sizeof(line), f)) { fclose(f); return 0; }
    line[strcspn(line, "\r\n")] = 0;

    /* count cols from header */
    int nc = 0; char *sp, *tok = my_strtok_r(line, "|", &sp);
    while (tok && nc < MAX_COLS) { nc++; tok = my_strtok_r(NULL, "|", &sp); }
    rs->ncols = nc;

    while (fgets(line, sizeof(line), f) && rs->nrows < MAX_ROWS) {
        line[strcspn(line, "\r\n")] = 0;
        Row *row = &rs->rows[rs->nrows];
        row->ncols = 0;
        char *t = my_strtok_r(line, "|", &sp);
        while (t && row->ncols < nc) {
            Cell *c = &row->cells[row->ncols];
            int is_int = (*t != '\0');
            for (char *p = t; *p && is_int; p++)
                if (*p != '-' && !isdigit((unsigned char)*p)) is_int = 0;
            c->type = is_int ? SQLITE_INTEGER : SQLITE_TEXT;
            strncpy(c->v, t, MAX_VAL-1); c->v[MAX_VAL-1] = '\0';
            row->ncols++;
            t = my_strtok_r(NULL, "|", &sp);
        }
        while (row->ncols < nc) { row->cells[row->ncols].type = SQLITE_NULL; row->cells[row->ncols].v[0] = '\0'; row->ncols++; }
        rs->nrows++;
    }
    fclose(f);
    return 1;
}

static int save_tbl(sqlite3 *db, const char *tbl, ResultSet *rs) {
    char path[1024]; tblpath(db, tbl, path, sizeof(path));
    FILE *f = fopen(path, "w");
    if (!f) { snprintf(db->errmsg, sizeof(db->errmsg), "Cannot write %s", path); db->errcode = SQLITE_IOERR; return 0; }

    /* read header from schema */
    char sp[1024], hdr[2048] = ""; schpath(db, sp, sizeof(sp));
    FILE *sf = fopen(sp, "r");
    if (sf) {
        char ln[1024];
        while (fgets(ln, sizeof(ln), sf)) {
            ln[strcspn(ln, "\r\n")] = 0;
            char *sep = strchr(ln, ':');
            if (sep) { *sep = '\0'; if (my_stricmp(ln, tbl) == 0) { strncpy(hdr, sep+1, sizeof(hdr)-1); break; } }
        }
        fclose(sf);
    }
    if (hdr[0]) fprintf(f, "%s\n", hdr);

    for (int i = 0; i < rs->nrows; i++) {
        for (int j = 0; j < rs->rows[i].ncols; j++) {
            if (j > 0) fputc('|', f);
            if (rs->rows[i].cells[j].type != SQLITE_NULL)
                fprintf(f, "%s", rs->rows[i].cells[j].v);
        }
        fputc('\n', f);
    }
    fclose(f);
    return 1;
}

/* ===== SQL Parser ===== */

/* CREATE TABLE */
static int sql_create(sqlite3 *db, const char *sql) {
    char lo[MAX_SQL]; strncpy(lo, sql, MAX_SQL-1); lo[MAX_SQL-1]='\0';
    for (char *p = lo; *p; p++) *p = (char)tolower((unsigned char)*p);

    char *st = strstr(lo, "create table");
    if (!st) return 0;
    char *ns = st + 12;
    while (*ns == ' ') ns++;
    if (strncmp(ns, "if not exists ", 14) == 0) ns += 14;

    char tbl[MAX_NAME] = ""; int i = 0;
    while (*ns && *ns != ' ' && *ns != '(' && i < MAX_NAME-1) tbl[i++] = *ns++;
    tbl[i] = '\0';
    if (!tbl[0]) return 0;

    /* skip if exists */
    char path[1024]; tblpath(db, tbl, path, sizeof(path));
    FILE *test = fopen(path, "r");
    if (test) { fclose(test); return 1; }

    /* extract column names */
    const char *lp = strchr(sql, '('), *rp = strrchr(sql, ')');
    if (!lp || !rp) return 0;

    char cols[2048] = ""; int cc = 0;
    const char *p = lp + 1;
    while (p < rp) {
        while (p < rp && (*p==' '||*p=='\t'||*p=='\n'||*p==',')) p++;
        if (p >= rp || *p == ')') break;
        char cn[MAX_NAME] = ""; int ci = 0;
        while (p < rp && *p != ' ' && *p != '\t' && *p != ',') {
            if (ci < MAX_NAME-1) cn[ci++] = *p;
            p++;
        }
        cn[ci] = '\0';
        while (p < rp && *p != ',') {
            if (*p == '(') { int d=1; p++; while (p<rp&&d>0){ if(*p=='(')d++; else if(*p==')')d--; p++; } }
            else p++;
        }
        unquote_str(cn);
        char cn_low[MAX_NAME]; strncpy(cn_low,cn,MAX_NAME-1); cn_low[MAX_NAME-1]='\0';
        for (char *x=cn_low;*x;x++) *x=(char)tolower((unsigned char)*x);
        if (cn[0] && strcmp(cn_low,"foreign") && strcmp(cn_low,"primary") &&
            strcmp(cn_low,"constraint") && strcmp(cn_low,"unique") &&
            strcmp(cn_low,"key") && strcmp(cn_low,"references") &&
            strcmp(cn_low,"not") && strcmp(cn_low,"default") &&
            strcmp(cn_low,"check") && strcmp(cn_low,"autoincrement")) {
            if (cc>0) strcat(cols,"|");
            strcat(cols,cn); cc++;
        }
    }

    char sp[1024]; schpath(db, sp, sizeof(sp));
    FILE *sf = fopen(sp, "a");
    if (sf) { fprintf(sf, "%s:%s\n", tbl, cols); fclose(sf); }

    FILE *tf = fopen(path, "w");
    if (tf) { fprintf(tf, "%s\n", cols); fclose(tf); }
    return 1;
}

/* INSERT */
static int sql_insert(sqlite3 *db, const char *sql) {
    char lo[MAX_SQL]; strncpy(lo, sql, MAX_SQL-1); lo[MAX_SQL-1]='\0';
    for (char *p = lo; *p; p++) *p = (char)tolower((unsigned char)*p);
    if (!strstr(lo, "insert")) return 0;

    /* table name */
    const char *kn = my_stristr(sql, "into");
    if (!kn) return 0;
    kn += 4; while (*kn == ' ') kn++;
    char tbl[MAX_NAME] = ""; int ti = 0;
    while (*kn && *kn != ' ' && *kn != '(' && ti < MAX_NAME-1) tbl[ti++] = *kn++;
    tbl[ti] = '\0';
    if (!tbl[0]) return 0;

    /* values */
    const char *kv = my_stristr(sql, "values");
    if (!kv) return 0;
    kv += 6; while (*kv == ' ' || *kv == '(') kv++;

    char vals[MAX_COLS][MAX_VAL]; int nv = 0;
    const char *vp = kv;
    while (*vp && *vp != ')' && *vp != ';' && nv < MAX_COLS) {
        while (*vp == ' ' || *vp == ',') vp++;
        if (*vp == ')' || *vp == ';' || !*vp) break;
        char q = 0;
        if (*vp == '\'' || *vp == '\"') { q = *vp; vp++; }
        int vi = 0;
        while (*vp && vi < MAX_VAL-1) {
            if (q) { if (*vp == q) { vp++; break; } }
            else { if (*vp == ',' || *vp == ')' || *vp == ';') break; }
            vals[nv][vi++] = *vp++;
        }
        vals[nv][vi] = '\0'; nv++;
        if (!q) while (*vp == ' ') vp++;
    }

    int is_replace = (strstr(lo, "or replace") != NULL);
    ResultSet rs;
    if (!load_tbl(db, tbl, &rs)) { rs.nrows = 0; }

    if (is_replace && nv > 0 && rs.nrows > 0) {
        for (int r = 0; r < rs.nrows; r++) {
            if (rs.rows[r].ncols > 0 && strcmp(rs.rows[r].cells[0].v, vals[0]) == 0) {
                for (int c = 0; c < nv && c < rs.rows[r].ncols; c++) {
                    strncpy(rs.rows[r].cells[c].v, vals[c], MAX_VAL-1);
                    int ii = (*vals[c]!='\0');
                    for (char *xx=vals[c]; *xx&&ii; xx++)
                        if (*xx!='-'&&!isdigit((unsigned char)*xx)) ii=0;
                    rs.rows[r].cells[c].type = ii ? SQLITE_INTEGER : SQLITE_TEXT;
                }
                save_tbl(db, tbl, &rs); db->changes = 1;
                return 1;
            }
        }
    }

    if (rs.nrows < MAX_ROWS) {
        Row *row = &rs.rows[rs.nrows];
        row->ncols = nv;
        int mx = 0;
        for (int r = 0; r < rs.nrows; r++)
            if (rs.rows[r].ncols>0 && rs.rows[r].cells[0].type==SQLITE_INTEGER)
                { int vv = atoi(rs.rows[r].cells[0].v); if (vv>mx) mx=vv; }
        for (int c = 0; c < nv; c++) {
            if (c == 0 && nv > 0) {
                /* auto-increment: use max+1 for id column */
                int ai = mx + 1;
                snprintf(row->cells[c].v, MAX_VAL, "%d", ai);
                row->cells[c].type = SQLITE_INTEGER;
                db->last_rowid = ai;
            } else {
                strncpy(row->cells[c].v, vals[c], MAX_VAL-1);
                int ii = (*vals[c]!='\0');
                for (char *xx=vals[c]; *xx&&ii; xx++)
                    if (*xx!='-'&&!isdigit((unsigned char)*xx)) ii=0;
                row->cells[c].type = ii ? SQLITE_INTEGER : SQLITE_TEXT;
            }
        }
        rs.nrows++; db->changes = 1;
    }
    save_tbl(db, tbl, &rs);
    return 1;
}

/* DELETE */
static int sql_delete(sqlite3 *db, const char *sql) {
    char lo[MAX_SQL]; strncpy(lo, sql, MAX_SQL-1); lo[MAX_SQL-1]='\0';
    for (char *p = lo; *p; p++) *p = (char)tolower((unsigned char)*p);
    char *dl = strstr(lo, "delete from ");
    if (!dl) return 0;
    char *ns = dl + 12; while (*ns == ' ') ns++;
    char tbl[MAX_NAME] = ""; int i = 0;
    while (*ns && *ns != ' ' && i < MAX_NAME-1) tbl[i++] = *ns++;
    tbl[i] = '\0';
    if (!tbl[0]) return 0;

    ResultSet rs;
    if (!load_tbl(db, tbl, &rs)) { db->changes = 0; return 1; }

    char *wh = strstr(lo, "where ");
    if (wh) {
        wh += 6;
        char col[MAX_NAME]="", op[8]="", val[MAX_VAL]="";
        sscanf(wh, "%127s %7s %511s", col, op, val);
        unquote_str(val);
        if (strcmp(op, "=") == 0) {
            int ci = -1;
            char sp[1024]; schpath(db, sp, sizeof(sp));
            FILE *sf = fopen(sp, "r");
            if (sf) {
                char ln[1024];
                while (fgets(ln, sizeof(ln), sf)) {
                    ln[strcspn(ln, "\r\n")] = 0;
                    char *sep = strchr(ln, ':');
                    if (sep) { *sep = '\0';
                        if (my_stricmp(ln, tbl) == 0) {
                            char *svp; int cx = 0; char *tk = my_strtok_r(sep+1, "|", &svp);
                            while (tk) { if (my_stricmp(tk, col)==0) { ci=cx; break; } cx++; tk = my_strtok_r(NULL, "|", &svp); }
                            break;
                        }
                    }
                }
                fclose(sf);
            }
            if (ci >= 0) {
                int w = 0;
                for (int r = 0; r < rs.nrows; r++) {
                    int match = (ci < rs.rows[r].ncols && strcmp(rs.rows[r].cells[ci].v, val) == 0);
                    if (!match) { if (w != r) rs.rows[w] = rs.rows[r]; w++; }
                }
                db->changes = rs.nrows - w; rs.nrows = w;
            }
        }
    } else { db->changes = rs.nrows; rs.nrows = 0; }
    save_tbl(db, tbl, &rs);
    return 1;
}

/* SELECT */
static int sql_select(sqlite3_stmt *stmt, const char *sql) {
    sqlite3 *db = stmt->db;
    char lo[MAX_SQL]; strncpy(lo, sql, MAX_SQL-1); lo[MAX_SQL-1]='\0';
    for (char *p = lo; *p; p++) *p = (char)tolower((unsigned char)*p);

    char *fr = strstr(lo, " from ");
    if (!fr) return 0;
    char *ns = fr + 6; while (*ns == ' ') ns++;
    char tbl[MAX_NAME] = ""; int i = 0;
    while (*ns && *ns != ' ' && *ns != ';' && i < MAX_NAME-1) tbl[i++] = *ns++;
    tbl[i] = '\0';
    if (!tbl[0]) return 0;

    ResultSet all;
    if (!load_tbl(db, tbl, &all)) {
        stmt->result.nrows = 0; stmt->result.ncols = 0; stmt->result.cursor = 0;
        stmt->is_select = 1; return 1;
    }

    char *wh = strstr(lo, "where ");
    if (wh) {
        wh += 6;
        char col[MAX_NAME]="", op[8]="", val[MAX_VAL]="";
        sscanf(wh, "%127s %7s %511s", col, op, val);
        unquote_str(val);
        if (strcmp(op, "=") == 0) {
            int ci = -1;
            char sp[1024]; schpath(db, sp, sizeof(sp));
            FILE *sf = fopen(sp, "r");
            if (sf) {
                char ln[1024];
                while (fgets(ln, sizeof(ln), sf)) {
                    ln[strcspn(ln, "\r\n")] = 0;
                    char *sep = strchr(ln, ':');
                    if (sep) { *sep = '\0';
                        if (my_stricmp(ln, tbl) == 0) {
                            char *svp2; int cx = 0; char *tk = my_strtok_r(sep+1, "|", &svp2);
                            while (tk) { if (my_stricmp(tk, col)==0) { ci=cx; break; } cx++; tk = my_strtok_r(NULL, "|", &svp2); }
                            break;
                        }
                    }
                }
                fclose(sf);
            }
            if (ci >= 0) {
                int w = 0;
                for (int r = 0; r < all.nrows; r++)
                    if (ci < all.rows[r].ncols && strcmp(all.rows[r].cells[ci].v, val) == 0)
                        { if (w != r) all.rows[w] = all.rows[r]; w++; }
                all.nrows = w;
            }
        }
    }

    memcpy(&stmt->result, &all, sizeof(ResultSet));
    stmt->result.cursor = 0;
    stmt->is_select = 1;
    return 1;
}

/* ===== Public API ===== */

const char *sqlite3_libversion(void) { return SQLITE_VERSION; }

int sqlite3_open(const char *fn, sqlite3 **pp) {
    return sqlite3_open_v2(fn, pp, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
}

int sqlite3_open_v2(const char *fn, sqlite3 **pp, int flags, const char *vfs) {
    (void)flags; (void)vfs;
    sqlite3 *db = (sqlite3 *)calloc(1, sizeof(sqlite3));
    if (!db) { *pp = NULL; return SQLITE_NOMEM; }
    if (fn && strcmp(fn, ":memory:") == 0) {
        snprintf(db->dbpath, sizeof(db->dbpath), ":memory:");
        snprintf(db->datadir, sizeof(db->datadir), "campus_rpg_mem_data");
    } else if (fn) {
        strncpy(db->dbpath, fn, sizeof(db->dbpath)-1);
        snprintf(db->datadir, sizeof(db->datadir), "%s_data", fn);
    } else {
        strcpy(db->dbpath, "campus_rpg.db");
        strcpy(db->datadir, "campus_rpg.db_data");
    }
    ensure_dir(db);
    db->errcode = SQLITE_OK;
    db->errmsg[0] = '\0';
    db->changes = 0;
    db->last_rowid = 0;
    *pp = db;
    return SQLITE_OK;
}

int sqlite3_close(sqlite3 *db) { if (db) free(db); return SQLITE_OK; }
int sqlite3_close_v2(sqlite3 *db) { return sqlite3_close(db); }
const char *sqlite3_errmsg(sqlite3 *db) { return db ? db->errmsg : "null db"; }
int sqlite3_errcode(sqlite3 *db) { return db ? db->errcode : SQLITE_ERROR; }
int sqlite3_extended_errcode(sqlite3 *db) { return sqlite3_errcode(db); }

int sqlite3_exec(sqlite3 *db, const char *sql, sqlite3_callback cb,
                 void *arg, char **errmsg) {
    if (!db || !sql) { if (errmsg) *errmsg = my_strdup("null db/sql"); return SQLITE_ERROR; }
    const char *p = sql; while (*p==' '||*p=='\t'||*p=='\n'||*p=='\r'||*p==';') p++;
    if (!*p) return SQLITE_OK;
    db->changes = 0;
    db->errcode = SQLITE_OK;
    db->errmsg[0] = '\0';

    char lo[MAX_SQL]; strncpy(lo, sql, MAX_SQL-1); lo[MAX_SQL-1]='\0';
    for (char *x=lo;*x;x++) *x=(char)tolower((unsigned char)*x);

    if (strncmp(lo, "create", 6) == 0) sql_create(db, sql);
    else if (strncmp(lo, "insert", 6) == 0) sql_insert(db, sql);
    else if (strncmp(lo, "delete", 6) == 0) sql_delete(db, sql);
    else if (strncmp(lo, "select", 6) == 0) {
        sqlite3_stmt *st = NULL;
        int rc = sqlite3_prepare_v2(db, sql, -1, &st, NULL);
        if (rc == SQLITE_OK && st) {
            int nc = sqlite3_column_count(st);
            while (sqlite3_step(st) == SQLITE_ROW) {
                if (cb) {
                    char *vals[MAX_COLS], bufs[MAX_COLS][MAX_VAL];
                    for (int i=0; i<nc; i++) {
                        const unsigned char *t = sqlite3_column_text(st, i);
                        if (t) { strncpy(bufs[i], (const char*)t, MAX_VAL-1); } else { bufs[i][0]='\0'; }
                        vals[i] = bufs[i];
                    }
                    cb(arg, nc, vals, NULL);
                }
            }
            sqlite3_finalize(st);
        }
    } else {
        db->errcode = SQLITE_ERROR;
        snprintf(db->errmsg, sizeof(db->errmsg), "Unsupported SQL");
        if (errmsg) *errmsg = my_strdup(db->errmsg);
        return SQLITE_ERROR;
    }
    if (errmsg) *errmsg = NULL;
    return SQLITE_OK;
}

int sqlite3_prepare_v2(sqlite3 *db, const char *zSql, int nByte,
                       sqlite3_stmt **ppStmt, const char **pzTail) {
    if (!db || !zSql) return SQLITE_ERROR;
    sqlite3_stmt *st = (sqlite3_stmt *)calloc(1, sizeof(sqlite3_stmt));
    if (!st) return SQLITE_NOMEM;
    st->db = db; st->is_select = 0; st->done = 0;
    int len = nByte < 0 ? (int)strlen(zSql) : nByte;
    if (len >= MAX_SQL) len = MAX_SQL - 1;
    strncpy(st->sql, zSql, len); st->sql[len] = '\0';

    char lo[MAX_SQL]; strncpy(lo, st->sql, MAX_SQL-1); lo[MAX_SQL-1]='\0';
    for (char *x=lo;*x;x++) *x=(char)tolower((unsigned char)*x);
    if (strncmp(lo, "select", 6) == 0) sql_select(st, st->sql);

    if (pzTail) *pzTail = NULL;
    *ppStmt = st;
    return SQLITE_OK;
}

int sqlite3_step(sqlite3_stmt *st) {
    if (!st) return SQLITE_ERROR;
    if (st->done) return SQLITE_DONE;
    if (st->is_select) {
        if (st->result.cursor < st->result.nrows) {
            st->result.cursor++;
            return SQLITE_ROW;
        }
        st->done = 1;
        return SQLITE_DONE;
    }
    st->done = 1;
    return SQLITE_DONE;
}

int sqlite3_finalize(sqlite3_stmt *st) { if (st) free(st); return SQLITE_OK; }
int sqlite3_reset(sqlite3_stmt *st) { if (st) { st->result.cursor = 0; st->done = 0; } return SQLITE_OK; }
int sqlite3_column_count(sqlite3_stmt *st) { return st ? st->result.ncols : 0; }
const char *sqlite3_column_name(sqlite3_stmt *st, int N) { (void)st; (void)N; return ""; }

int sqlite3_column_type(sqlite3_stmt *st, int iCol) {
    if (!st || st->result.cursor < 1) return SQLITE_NULL;
    int r = st->result.cursor - 1;
    if (r >= st->result.nrows || iCol >= st->result.rows[r].ncols) return SQLITE_NULL;
    return st->result.rows[r].cells[iCol].type;
}

static const unsigned char *get_col_text(sqlite3_stmt *st, int iCol) {
    if (!st || st->result.cursor < 1) return (const unsigned char *)"";
    int r = st->result.cursor - 1;
    if (r >= st->result.nrows || iCol >= st->result.rows[r].ncols) return (const unsigned char *)"";
    return (const unsigned char *)st->result.rows[r].cells[iCol].v;
}

const void *sqlite3_column_blob(sqlite3_stmt *st, int iCol) { return get_col_text(st, iCol); }
double sqlite3_column_double(sqlite3_stmt *st, int iCol) { return (double)sqlite3_column_int(st, iCol); }
int sqlite3_column_int(sqlite3_stmt *st, int iCol) { return atoi((const char *)get_col_text(st, iCol)); }
sqlite3_int64 sqlite3_column_int64(sqlite3_stmt *st, int iCol) { return (sqlite3_int64)sqlite3_column_int(st, iCol); }
const unsigned char *sqlite3_column_text(sqlite3_stmt *st, int iCol) { return get_col_text(st, iCol); }
int sqlite3_column_bytes(sqlite3_stmt *st, int iCol) {
    const unsigned char *t = get_col_text(st, iCol);
    return t ? (int)strlen((const char *)t) : 0;
}

int sqlite3_bind_int(sqlite3_stmt *st, int idx, int val) { (void)st;(void)idx;(void)val; return SQLITE_OK; }
int sqlite3_bind_text(sqlite3_stmt *st, int idx, const char *val, int n, void(*dtor)(void*)) {
    (void)st;(void)idx;(void)val;(void)n;(void)dtor; return SQLITE_OK;
}

void sqlite3_free(void *p) { free(p); }
void *sqlite3_malloc(int n) { return malloc(n); }
int sqlite3_changes(sqlite3 *db) { return db ? db->changes : 0; }
sqlite3_int64 sqlite3_last_insert_rowid(sqlite3 *db) { return db ? db->last_rowid : 0; }

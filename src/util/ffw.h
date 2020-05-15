/*
 * ffw.h : Fontforge Wrapper
 *
 * Processing fonts using Fontforge
 *
 * fontforge.h cannot be included in C++
 * So this wrapper in C publishes several functions we need
 *
 * by WangLu
 * 2012.09.03
 */

#ifndef PDFTOHTMLEX_UTIL_FWW_H
#define PDFTOHTMLEX_UTIL_FWW_H
#ifdef __cplusplus
#include <cstdint>
namespace pdf2htmlEX {
extern "C" {
#else
#include <stdint.h>
#endif



////////////////////////
// Forward declarations

typedef struct splinefont SplineFont;
typedef struct enc Encoding;
typedef struct dbounds DBounds;
typedef struct fontviewbase FontViewBase;
typedef struct encmap EncMap;
typedef struct namelist NameList;


extern void FindProgDir(char *prog);
extern void InitSimpleStuff(void);
extern Encoding *FindOrMakeEncoding(const char *name);
extern void SplineFontFindBounds(SplineFont *sf, DBounds *bounds);
extern SplineFont *SplineFontNew(void);
extern void FVRemoveKerns(FontViewBase *fv);
extern void FVRemoveVKerns(FontViewBase *fv);
extern void AltUniFree(struct altuni *altuni);
extern int GenerateScript(SplineFont *sf, char *filename, const char *bitmaptype, int fmflags, int res, char *subfontdirectory, struct sflist *sfs, EncMap *map, NameList *rename_to, int layer);
extern int SFForceEncoding(SplineFont *sf, EncMap *old, Encoding *new_enc);
extern void EncMapFree(EncMap *map);
extern EncMap *EncMapFromEncoding(SplineFont *sf, Encoding *enc);
extern void SFReplaceEncodingBDFProps(SplineFont *sf, EncMap *map);
extern void SFDefaultOS2Info(struct pfminfo *pfminfo, SplineFont *sf, char *fontname);
extern int SFFindSlot(SplineFont *sf, EncMap *map, int unienc, const char *name);
extern int FVImportImages(FontViewBase *fv, char *path, int format, int toback, int flags);
extern void SFConvertToOrder2(SplineFont *_sf);

// changed api because enums can't be forwarded
// extern int UniFromName(const char *name, enum uni_interp interp, Encoding *encname); -> cant forward declaration of enums
// extern const char *StdGlyphName(char *buffer, int uni, enum uni_interp interp, NameList *for_this_font); -> cant forward declaration of enums
extern int UniFromName(const char *name, int interp, Encoding *encname);
extern const char *StdGlyphName(char *buffer, int uni, int interp, NameList *for_this_font);


////////////////////////
// global
void ffw_init(int debug, const char* program_name);
void ffw_finalize(void);
const char * ffw_get_version(void);

////////////////////////
// load & save
void ffw_new_font();
void ffw_load_font(const char * filename);
void ffw_prepare_font(void);

void ffw_save(const char * filename);
void ffw_close(void);

////////////////////////
// encoding
void ffw_reencode_glyph_order(void);
void ffw_reencode_unicode_full(void);
void ffw_reencode_raw(int32_t * mapping, int mapping_len, int force);
void ffw_reencode_raw2(char ** mapping, int mapping_len, int force);

void ffw_cidflatten(void);
// add a new empty char into the font
void ffw_add_empty_char(int32_t unicode, int width);

////////////////////////
// metrics
int ffw_get_em_size(void);
// manipulate ascent and descent
// ascent is between 0 and 1
// descent is between -1 and 0
void ffw_fix_metric();
// get ascent/descent based on the shape
void ffw_get_metric(double * ascent, double * descent);
// set corresponding fields
void ffw_set_metric(double ascent, double descent);

void ffw_set_widths(int * width_list, int mapping_len, 
        int stretch_narrow, int squeeze_wide);

////////////////////////
// others
// (ox,oy) is the position of the true origin, fractions related to em_size
// also true for glyph_width
void ffw_import_svg_glyph(int code, const char * filename, double ox, double oy, double glyph_width);
void ffw_auto_hint(void);
void ffw_override_fstype(void);

#ifdef __cplusplus
}
}
#endif
#endif

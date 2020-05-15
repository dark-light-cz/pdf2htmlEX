/*
 * HTMLRenderer.h
 *
 * Copyright (C) 2012,2013 Lu Wang <coolwanglu@gmail.com>
 */

#ifndef HTMLRENDERER_H_
#define HTMLRENDERER_H_

#include <unordered_map>
#include <cstdint>
#include <fstream>
#include <memory>

#include <OutputDev.h>
#include <GfxState.h>
#include <Stream.h>
#include <PDFDoc.h>
// goo/gtypes not present in newer version of poppler => need new imports
// #include <goo/gtypes.h>
#include <Outline.h>
//
#include <Object.h>
#include <GfxFont.h>
#include <Annot.h>

// for form.cc
#include <Page.h>
#include <Form.h>

#include "pdf2htmlEX-config.h"

#include "Param.h"
#include "Preprocessor.h"
#include "StringFormatter.h"
#include "TmpFiles.h"
#include "Color.h"
#include "StateManager.h"
#include "HTMLTextPage.h"

#include "BackgroundRenderer/BackgroundRenderer.h"
#include "CoveredTextDetector.h"
#include "DrawingTracer.h"

#include "util/const.h"
#include "util/misc.h"


namespace pdf2htmlEX {

struct HTMLRenderer : OutputDev
{
    HTMLRenderer(const Param & param, const char* program_name);
    virtual ~HTMLRenderer();

    void process(PDFDoc * doc);

    ////////////////////////////////////////////////////
    // OutputDev interface
    ////////////////////////////////////////////////////
    
    // Does this device use upside-down coordinates?
    // (Upside-down means (0,0) is the top left corner of the page.)
    virtual bool upsideDown() { return false; }

    // Does this device use drawChar() or drawString()? false means use drawString
    virtual bool useDrawChar() { return false; }
    
    // Does this device use tilingPatternFill()?  If this returns false,
    // tiling pattern fills will be reduced to a series of other drawing
    // operations.
    // virtual bool useTilingPatternFill() { return false; }

    // Does this device use functionShadedFill(), axialShadedFill(), and
    // radialShadedFill()?  If this returns false, these shaded fills
    // will be reduced to a series of other drawing operations.
    virtual bool useShadedFills(int type) { return (type == 2) ? true: false; }

    // Does this device use drawForm()?  If this returns false,
    // form-type XObjects will be interpreted (i.e., unrolled).
    // virtual bool useDrawForm() { return false; }

    // Does this device use beginType3Char/endType3Char?  Otherwise,
    // text in Type 3 fonts will be drawn with drawChar/drawString.
    // We use drawString which desides how to interpret this depending on param
    virtual bool interpretType3Chars() { return false; }

    // Does this device need non-text content?
    virtual bool needNonText() { return (param.process_nontext) ? true: false; }
    
    // Does this device require incCharCount to be called for text on
    // non-shown layers?
    // virtual bool needCharCount() { return false; }

    // Does this device need to clip pages to the crop box even when the
    // box is the crop box?
    virtual bool needClipToCropBox() { return true; }

    //----- initialization and control

    // Set default transform matrix.
    virtual void setDefaultCTM(const double *ctm);
    
    // Check to see if a page slice should be displayed.  If this
    // returns false, the page display is aborted.  Typically, an
    // OutputDev will use some alternate means to display the page
    // before returning false.
    // virtual bool checkPageSlice(Page *page, double hDPI, double vDPI,
    //            int rotate, bool useMediaBox, bool crop,
    //            int sliceX, int sliceY, int sliceW, int sliceH,
    //            bool printing,
    //            bool (* abortCheckCbk)(void *data) = nullptr,
    //            void * abortCheckCbkData = nullptr,
    //            bool (*annotDisplayDecideCbk)(Annot *annot, void *user_data) = nullptr,
    //            void *annotDisplayDecideCbkData = nullptr)
    //  { return false; /* default in poppler/OutputDev.h is true */ }

    // Start a page.
    virtual void startPage(int pageNum, GfxState *state, XRef * xref);

    // End a page.
    virtual void endPage();
    
    // Dump page contents to display.
    // virtual void dump() {}
    
    //----- coordinate conversion

    // Convert between device and user coordinates.
    // virtual void cvtDevToUser(double dx, double dy, double *ux, double *uy);
    // virtual void cvtUserToDev(double ux, double uy, int *dx, int *dy);

    // const double *getDefCTM() const { return defCTM; }
    // const double *getDefICTM() const { return defICTM; }
    
    /*
     * To optimize false alarms
     * We just mark as changed, and recheck if they have been changed when we are about to output a new string
     */
    
    //----- save/restore graphics state
    virtual void saveState(GfxState *state);
    virtual void restoreState(GfxState * state);

    //----- update graphics state
    virtual void updateAll(GfxState * state);
  
    // Update the Current Transformation Matrix (CTM), i.e., the new matrix
    // given in m11, ..., m32 is combined with the current value of the CTM.
    // At the same time, when this method is called, state->getCTM() already
    // contains the correct new CTM, so one may as well replace the
    // CTM of the renderer with that.
    virtual void updateCTM(GfxState * state, double m11, double m12, 
            double m21, double m22, double m31, double m32);

    // virtual void updateLineDash(GfxState * state);
    // virtual void updateFlatness(GfxState * state);
    // virtual void updateLineJoin(GfxState * state);
    // virtual void updateLineCap(GfxState * state);
    // virtual void updateMiterLimit(GfxState * state);
    // virtual void updateLineWidth(GfxState * state);
    // virtual void updateStrokeAdjust(GfxState * state);
    // virtual void updateAlphaIsShape(GfxState * state);
    // virtual void updateTextKnockout(GfxState * state);
    virtual void updateFillColorSpace(GfxState * state); // original pdf2htlEx
    virtual void updateStrokeColorSpace(GfxState * state); // original pdf2htlEx
    virtual void updateFillColor(GfxState * state); // original pdf2htlEx
    virtual void updateStrokeColor(GfxState * state); // original pdf2htlEx
    // virtual void updateBlendMode(GfxState * state);
    // virtual void updateFillOpacity(GfxState * state);
    // virtual void updateStrokeOpacity(GfxState * state);
    // virtual void updatePatternOpacity(GfxState * state);
    // virtual void clearPatternOpacity(GfxState * state);
    // virtual void updateFillOverprint(GfxState * state);
    // virtual void updateStrokeOverprint(GfxState * state);
    // virtual void updateOverprintMode(GfxState * state);
    // virtual void updateTransfer(GfxState * state);
    // virtual void updateFillColorStop(GfxState * state, double offset);

    //----- update text state
    virtual void updateFont(GfxState * state); // original pdf2htlEx
    virtual void updateTextMat(GfxState * state); // original pdf2htlEx
    virtual void updateCharSpace(GfxState * state); // original pdf2htlEx
    virtual void updateRender(GfxState * state); // original pdf2htlEx
    virtual void updateRise(GfxState * state); // original pdf2htlEx
    virtual void updateWordSpace(GfxState * state); // original pdf2htlEx
    virtual void updateHorizScaling(GfxState * state); // original pdf2htlEx
    virtual void updateTextPos(GfxState * state); // original pdf2htlEx
    virtual void updateTextShift(GfxState * state, double shift); // original pdf2htlEx
    // virtual void saveTextPos(GfxState * state);
    // virtual void restoreTextPos(GfxState * state);

    /*
     * Rendering
     */
    
    //----- path painting
    virtual void stroke(GfxState *state);
    virtual void fill(GfxState *state);
    virtual void eoFill(GfxState *state);
    // virtual bool tilingPatternFill(GfxState * /*state*/, Gfx * /*gfx*/, Catalog * /*cat*/, Object * /*str*/,
    //                 const double * /*pmat*/, int /*paintType*/, int /*tilingType*/, Dict * /*resDict*/,
    //                 const double * /*mat*/, const double * /*bbox*/,
    //                 int /*x0*/, int /*y0*/, int /*x1*/, int /*y1*/,
    //                 double /*xStep*/, double /*yStep*/)
    // { return false; }
    // virtual bool functionShadedFill(GfxState * /*state*/,
    //            GfxFunctionShading * /*shading*/)
    //   { return false; }
    virtual bool axialShadedFill(GfxState *state, GfxAxialShading *shading, double tMin, double tMax);
    // virtual bool axialShadedSupportExtend(GfxState * /*state*/, GfxAxialShading * /*shading*/)
    //   { return false; }
    // virtual bool radialShadedFill(GfxState * /*state*/, GfxRadialShading * /*shading*/, double /*sMin*/, double /*sMax*/)
    //   { return false; }
    // virtual bool radialShadedSupportExtend(GfxState * /*state*/, GfxRadialShading * /*shading*/)
    //   { return false; }
    // virtual bool gouraudTriangleShadedFill(GfxState *state, GfxGouraudTriangleShading *shading)
    //   { return false; }
    // virtual bool patchMeshShadedFill(GfxState *state, GfxPatchMeshShading *shading)
    //   { return false; }


    //----- path clipping

    // Update the clipping path.  The new path is the intersection of the old path
    // with the path given in 'state'.
    // Additionally, set the clipping mode to the 'nonzero winding number rule'.
    // That is, a point is inside the clipping region if its winding number
    // with respect to the clipping path is nonzero.
    virtual void clip(GfxState * state);

    // Update the clipping path.  The new path is the intersection of the old path
    // with the path given in 'state'.
    // Additionally, set the clipping mode to the 'even-odd rule'.  That is, a point is
    // inside the clipping region if a ray from it to infinity will cross the clipping
    // path an odd number of times (disregarding the path orientation).
    virtual void eoClip(GfxState * state);
    virtual void clipToStrokePath(GfxState * state);
    
    //----- text drawing
    // virtual void beginStringOp(GfxState * /*state*/) {}
    // virtual void endStringOp(GfxState * /*state*/) {}
    // virtual void beginString(GfxState * /*state*/, const GooString * /*s*/) {}
    // virtual void endString(GfxState * /*state*/) {}

    // Draw one glyph at a specified position
    //
    // Arguments are:
    // CharCode code: This is the character code in the content stream. It needs to be mapped back to a glyph index.
    // int nBytes: The text strings in the content stream can consists of either 8-bit or 16-bit
    //             character codes depending on the font. nBytes is the number of bytes in the character code.
    // Unicode *u: The UCS-4 mapping used for text extraction (TextOutputDev).
    // int uLen: The number of unicode entries in u.  Usually '1', for a single character,
    //           but it may also have larger values, for example for ligatures.
    // virtual void drawChar(GfxState * /*state*/, double /*x*/, double /*y*/,
    //         double /*dx*/, double /*dy*/,
    //         double /*originX*/, double /*originY*/,
    //         CharCode /*code*/, int /*nBytes*/, const Unicode * /*u*/, int /*uLen*/) {}
    virtual void drawString(GfxState * state, const GooString * s);
    // virtual bool beginType3Char(GfxState * /*state*/, double /*x*/, double /*y*/,
    //              double /*dx*/, double /*dy*/,
    //              CharCode /*code*/, const Unicode * /*u*/, int /*uLen*/);
    // virtual void endType3Char(GfxState * /*state*/) {}
    // virtual void beginTextObject(GfxState * /*state*/) {}
    // virtual void endTextObject(GfxState * /*state*/) {}
    // virtual void incCharCount(int /*nChars*/) {}
    // virtual void beginActualText(GfxState * /*state*/, const GooString * /*text*/ ) {}
    // virtual void endActualText(GfxState * /*state*/) {}

    //----- image drawing
    // Draw an image mask.  An image mask is a one-bit-per-pixel image, where each pixel
    // can only be 'fill color' or 'transparent'.
    //
    // If 'invert' is false, a sample value of 0 marks the page with the current color,
    // and a 1 leaves the previous contents unchanged. If 'invert' is true, these meanings are reversed.
    // virtual void drawImageMask(GfxState *state, Object *ref, Stream *str,
    //                int width, int height, bool invert, bool interpolate,
    //                bool inlineImg);
    // virtual void setSoftMaskFromImageMask(GfxState *state,
    //           Object *ref, Stream *str,
    //           int width, int height, bool invert,
    //           bool inlineImg, double *baseMatrix);
    // virtual void unsetSoftMaskFromImageMask(GfxState *state, double *baseMatrix);
    virtual void drawImage(GfxState * state, Object * ref, Stream * str, 
            int width, int height, GfxImageColorMap * colorMap, 
            bool interpolate, const int *maskColors, bool inlineImg);

    // virtual void drawMaskedImage(GfxState *state, Object *ref, Stream *str,
    //               int width, int height,
    //               GfxImageColorMap *colorMap, bool interpolate,
    //               Stream *maskStr, int maskWidth, int maskHeight,
    //               bool maskInvert, bool maskInterpolate);
    virtual void drawSoftMaskedImage(GfxState *state, Object *ref, Stream *str,
                       int width, int height,
                       GfxImageColorMap *colorMap,
                       bool interpolate,
                       Stream *maskStr,
                       int maskWidth, int maskHeight,
                       GfxImageColorMap *maskColorMap,
                       bool maskInterpolate);

    //     //----- grouping operators
    // 
    //     virtual void endMarkedContent(GfxState *state);
    //     virtual void beginMarkedContent(const char *name, Dict *properties);
    //     virtual void markPoint(const char *name);
    //     virtual void markPoint(const char *name, Dict *properties);
    // 
    // 
    // 
    //     // ifdef OPI_SUPPORT
    //     //----- OPI functions
    //     virtual void opiBegin(GfxState *state, Dict *opiDict);
    //     virtual void opiEnd(GfxState *state, Dict *opiDict);
    //     // endif
    // 
    //     //----- Type 3 font operators
    //     virtual void type3D0(GfxState * /*state*/, double /*wx*/, double /*wy*/) {}
    //     virtual void type3D1(GfxState * /*state*/, double /*wx*/, double /*wy*/,
    //              double /*llx*/, double /*lly*/, double /*urx*/, double /*ury*/) {}
    // 
    //     //----- form XObjects
    //     virtual void drawForm(Ref /*id*/) {}
    // 
    //     //----- PostScript XObjects
    //     virtual void psXObject(Stream * /*psStream*/, Stream * /*level1Stream*/) {}
    // 
    //     //----- Profiling
    //     void startProfile();
    //     std::unordered_map<std::string, ProfileData>* getProfileHash() const { return profileHash.get(); }
    //     std::unique_ptr<std::unordered_map<std::string, ProfileData>> endProfile();
    // 
    //     //----- transparency groups and soft masks
    //     virtual bool checkTransparencyGroup(GfxState * /*state*/, bool /*knockout*/) { return true; }
    //     virtual void beginTransparencyGroup(GfxState * /*state*/, const double * /*bbox*/,
    //                 GfxColorSpace * /*blendingColorSpace*/,
    //                 bool /*isolated*/, bool /*knockout*/,
    //                 bool /*forSoftMask*/) {}
    //     virtual void endTransparencyGroup(GfxState * /*state*/) {}
    //     virtual void paintTransparencyGroup(GfxState * /*state*/, const double * /*bbox*/) {}
    //     virtual void setSoftMask(GfxState * /*state*/, const double * /*bbox*/, bool /*alpha*/,
    //            Function * /*transferFunc*/, GfxColor * /*backdropColor*/) {}
    //     virtual void clearSoftMask(GfxState * /*state*/) {}

    //----- links
    virtual void processLink(AnnotLink * al);

    // virtual bool getVectorAntialias() { return false; }
    // virtual void setVectorAntialias(bool /*vaa*/) {}
    
    // ==== OUR FUNCTIONS STARTS HERE ========= //

    /*
     * Covered text handling.
     */
    // Is a char (actually a glyph) covered by non-char's. Index in drawing order in current page.
    // Does not fail on out-of-bound conditions, but return false.
    bool is_char_covered(int index);
    // Currently drawn char (glyph) count in current page.
    int get_char_count() { return (int)covered_text_detector.get_chars_covered().size(); }

protected:
    ////////////////////////////////////////////////////
    // misc
    ////////////////////////////////////////////////////
    void pre_process(PDFDoc * doc);
    void post_process(void);

    void process_outline(void);
    void process_outline_items(const std::vector<OutlineItem*> * items);

    void process_form(std::ofstream & out);
    
    void set_stream_flags (std::ostream & out);

    void dump_css(void);

    // convert a LinkAction to a string that our Javascript code can understand
    std::string get_linkaction_str(LinkAction *, std::string & detail);

    ////////////////////////////////////////////////////
    /*
     * manage fonts
     *
     * In PDF: (install_*)
     * embedded font: fonts embedded in PDF
     * external font: fonts that have only names provided in PDF, the viewer should find a local font to match with
     *
     * In HTML: (export_*)
     * remote font: to be retrieved from the web server
     * remote default font: fallback styles for invalid fonts
     * local font: to be substituted with a local (client side) font
     */
    ////////////////////////////////////////////////////
    std::string dump_embedded_font(GfxFont * font, FontInfo & info);
    std::string dump_type3_font(GfxFont * font, FontInfo & info);
    void embed_font(const std::string & filepath, GfxFont * font, FontInfo & info, bool get_metric_only = false);
    const FontInfo * install_font(GfxFont * font);
    void install_embedded_font(GfxFont * font, FontInfo & info);
    void install_external_font (GfxFont * font, FontInfo & info);
    void export_remote_font(const FontInfo & info, const std::string & suffix, GfxFont * font);
    void export_remote_default_font(long long fn_id);
    void export_local_font(const FontInfo & info, GfxFont * font, const std::string & original_font_name, const std::string & cssfont);

    // depending on --embed***, to embed the content or add a link to it
    // "type": specify the file type, usually it's the suffix, in which case this parameter could be ""
    // "copy": indicates whether to copy the file into dest_dir, if not embedded
    void embed_file(std::ostream & out, const std::string & path, const std::string & type, bool copy);

    ////////////////////////////////////////////////////
    // state tracking 
    ////////////////////////////////////////////////////
    // reset all states
    void reset_state();
    // reset all ***_changed flags
    void reset_state_change();
    // check updated states, and determine new_line_status
    // make sure this function can be called several times consecutively without problem
    void check_state_change(GfxState * state);
    // prepare the line context, (close old tags, open new tags)
    // make sure the current HTML style consistent with PDF
    void prepare_text_line(GfxState * state);

    ////////////////////////////////////////////////////
    // PDF stuffs
    ////////////////////////////////////////////////////
    
    XRef * xref;
    PDFDoc * cur_doc;
    Catalog * cur_catalog;
    int pageNum;

    double default_ctm[6];

    /*
     * The content of each page is first scaled with factor1 (>=1), then scale back with factor2(<=1)
     *
     * factor1 is use to multiplied with all metrics (height/width/font-size...), in order to improve accuracy
     * factor2 is applied with css transform, and is exposed to Javascript
     *
     * factor1 & factor 2 are determined according to zoom and font-size-multiplier
     *
     */
    double text_zoom_factor (void) const { return text_scale_factor1 * text_scale_factor2; }
    double text_scale_factor1;
    double text_scale_factor2;

    // 1px on screen should be printed as print_scale()pt
    double print_scale (void) const { return 96.0 / DEFAULT_DPI / text_zoom_factor(); }


    const Param & param;

    ////////////////////////////////////////////////////
    // PDF states
    ////////////////////////////////////////////////////
    // track the original (unscaled) values to determine scaling and merge lines
    // current position
    double cur_tx, cur_ty; // real text position, in text coords
    double cur_font_size;
    // this is CTM * TextMAT in PDF
    // as we'll calculate the position of the origin separately
    double cur_text_tm[6]; // unscaled

    bool all_changed;
    bool ctm_changed;
    bool rise_changed;
    bool font_changed;
    bool text_pos_changed; 
    bool text_mat_changed;
    bool fill_color_changed;
    bool hori_scale_changed;
    bool word_space_changed;
    bool letter_space_changed;
    bool stroke_color_changed;
    bool clip_changed;

    ////////////////////////////////////////////////////
    // HTML states
    ////////////////////////////////////////////////////

    // optimize for web
    // we try to render the final font size directly
    // to reduce the effect of ctm as much as possible
    
    // the actual tm used is `real tm in PDF` scaled by 1/draw_text_scale, 
    // so everything rendered should be multiplied by draw_text_scale
    double draw_text_scale; 

    // the position of next char, in text coords
    // this is actual position (in HTML), which might be different from cur_tx/ty (in PDF)
    // also keep in mind that they are not the final position, as they will be transform by CTM (also true for cur_tx/ty)
    double draw_tx, draw_ty; 


    ////////////////////////////////////////////////////
    // styles & resources
    ////////////////////////////////////////////////////
    // managers store values actually used in HTML (i.e. scaled)
    std::unordered_map<long long, FontInfo> font_info_map;
    AllStateManager all_manager;
    HTMLTextState cur_text_state;
    HTMLLineState cur_line_state;
    HTMLClipState cur_clip_state;

    HTMLTextPage html_text_page;

    enum NewLineState
    {
        NLS_NONE,
        NLS_NEWSTATE, 
        NLS_NEWLINE,
        NLS_NEWCLIP
    } new_line_state;
    
    // for font reencoding
    std::vector<int32_t> cur_mapping; 
    std::vector<char*> cur_mapping2;
    std::vector<int> width_list; // width of each char

    Preprocessor preprocessor;

    // manage temporary files
    TmpFiles tmp_files;

    // for string formatting
    StringFormatter str_fmt;

    // render background image
    friend class SplashBackgroundRenderer; // ugly!
#if ENABLE_SVG
    friend class CairoBackgroundRenderer; // ugly!
#endif

    std::unique_ptr<BackgroundRenderer> bg_renderer, fallback_bg_renderer;

    struct {
        std::ofstream fs;
        std::string path;
    } f_outline, f_pages, f_css;
    std::ofstream * f_curpage;
    std::string cur_page_filename;

    static const std::string MANIFEST_FILENAME;

    CoveredTextDetector covered_text_detector;
    DrawingTracer tracer;
};

} //namespace pdf2htmlEX

#endif /* HTMLRENDERER_H_ */

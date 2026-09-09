#include <librsvg/rsvg.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <stdint.h>
#include <string.h>

static void check_text(void)
{
    const char *svg = "<svg xmlns='http://www.w3.org/2000/svg' width='128' height='32'>"
        "<text x='2' y='25' font-family='serif' font-size='24' fill='#008000'>ARM64</text></svg>";
    GError *error = NULL;
    RsvgHandle *handle = rsvg_handle_new_from_data((const guint8 *)svg, strlen(svg), &error);
    g_assert_no_error(error);
    g_assert_nonnull(handle);
    cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 128, 32);
    cairo_t *cr = cairo_create(surface);
    const RsvgRectangle viewport = {0, 0, 128, 32};
    g_assert_true(rsvg_handle_render_document(handle, cr, &viewport, &error));
    g_assert_no_error(error);
    g_assert_cmpint(cairo_status(cr), ==, CAIRO_STATUS_SUCCESS);
    cairo_surface_flush(surface);
    int ink = 0;
    for (int y = 0; y < 32; ++y) {
        const uint32_t *row = (const uint32_t *)(cairo_image_surface_get_data(surface)
            + y * cairo_image_surface_get_stride(surface));
        for (int x = 0; x < 128; ++x)
            if (row[x] >> 24) {
                g_assert_cmphex(row[x] & 0x00ff00ffu, ==, 0);
                ++ink;
            }
    }
    g_assert_cmpint(ink, >, 10);
    g_assert_cmpint(ink, <, 128 * 32);
    cairo_destroy(cr);
    cairo_surface_destroy(surface);
    g_object_unref(handle);
    g_print("PASS: installed SVG text shaping and rendering\n");
}

static void check_pixbuf(const char *path)
{
    GError *error = NULL;
    GdkPixbuf *image = gdk_pixbuf_new_from_file(path, &error);
    g_assert_no_error(error);
    g_assert_nonnull(image);
    g_assert_cmpint(gdk_pixbuf_get_width(image), ==, 4);
    g_assert_cmpint(gdk_pixbuf_get_height(image), ==, 4);
    int channels = gdk_pixbuf_get_n_channels(image);
    g_assert_true(channels == 3 || channels == 4);
    for (int y = 0; y < 4; ++y)
        for (int x = 0; x < 4; ++x) {
            const guchar *pixel = gdk_pixbuf_read_pixels(image)
                + y * gdk_pixbuf_get_rowstride(image) + x * channels;
            g_assert_cmpint(pixel[0], ==, x < 2 ? 255 : 0);
            g_assert_cmpint(pixel[1], ==, 0);
            g_assert_cmpint(pixel[2], ==, x < 2 ? 0 : 255);
            if (channels == 4)
                g_assert_cmpint(pixel[3], ==, 255);
        }
    g_object_unref(image);
    g_print("PASS: installed image decoding and exact pixels: %s\n", path);
}

int main(void)
{
    GError *error = NULL;
    RsvgHandle *handle = rsvg_handle_new_from_file("test.svg", &error);
    g_assert_no_error(error);
    g_assert_nonnull(handle);
    cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 4, 4);
    cairo_t *cr = cairo_create(surface);
    const RsvgRectangle viewport = {0, 0, 4, 4};
    g_assert_true(rsvg_handle_render_document(handle, cr, &viewport, &error));
    g_assert_no_error(error);
    g_assert_cmpint(cairo_status(cr), ==, CAIRO_STATUS_SUCCESS);
    cairo_surface_flush(surface);
    for (int y = 0; y < 4; ++y) {
        const uint32_t *row = (const uint32_t *)(cairo_image_surface_get_data(surface)
            + y * cairo_image_surface_get_stride(surface));
        for (int x = 0; x < 4; ++x)
            g_assert_cmphex(row[x], ==, x < 2 ? 0xffff0000u : 0xff0000ffu);
    }
    cairo_destroy(cr);
    cairo_surface_destroy(surface);
    g_object_unref(handle);
    g_print("PASS: installed librsvg C API rendering and exact pixels\n");
    check_pixbuf("test.svg");
    check_pixbuf("converted.png");
    check_text();
    return 0;
}

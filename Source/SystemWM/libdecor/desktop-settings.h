#if KLEMMUI_WITH_WAYLAND && !KLEMMUI_CUSTOM_SYSTEMWM
/**
* For some reason, this isn't part of the public libdecor api?
* At least the debian package doesn't include any of the headers for this,
* and libdecor-0.so doesn't export any of these.
*/

/*
 * Copyright © 2019 Christian Rauch
 * Copyright © 2024 Colin Kinloch
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial
 * portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#include <stdbool.h>

enum libdecor_color_scheme
{
	LIBDECOR_COLOR_SCHEME_DEFAULT,
	LIBDECOR_COLOR_SCHEME_PREFER_DARK,
	LIBDECOR_COLOR_SCHEME_PREFER_LIGHT,
};

bool
libdecor_get_cursor_settings(char** theme, int* size);

enum libdecor_color_scheme
	libdecor_get_color_scheme();

#endif
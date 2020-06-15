/*
 * Copyright (c) 2010 VMware, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * on the rights to use, copy, modify, merge, publish, distribute, sub
 * license, and/or sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NON-INFRINGEMENT.  IN NO EVENT SHALL VMWARE AND/OR THEIR SUPPLIERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/**
 * @file api-teximage.c
 *
 * Tests GL_EXT_texture_integer's error behavior with glTexImage2D().
 */

#include "piglit-util-gl.h"

PIGLIT_GL_TEST_CONFIG_BEGIN

	config.supports_gl_compat_version = 10;
	config.window_visual = PIGLIT_GL_VISUAL_RGBA | PIGLIT_GL_VISUAL_DOUBLE;
	config.khr_no_error_support = PIGLIT_HAS_ERRORS;

PIGLIT_GL_TEST_CONFIG_END

enum piglit_result
piglit_display(void)
{
	static const float black[4] = {0, 0, 0, 0};
	GLuint tex;

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	/* Check that GL_FLOAT type is not accepted with integer formats */
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16UI_EXT, 1, 1, 0,
		     GL_RGBA_INTEGER, GL_FLOAT, black);
	if (!piglit_check_gl_error(GL_INVALID_ENUM))
		piglit_report_result(PIGLIT_FAIL);

	/* Check that GL_INVALID_OPERATION is generated by trying to mix
	 * integer/float formats/types.
	 */
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 4, 4, 0,
		     GL_RGBA_INTEGER, GL_SHORT, NULL);
	if (!piglit_check_gl_error(GL_INVALID_OPERATION))
		piglit_report_result(PIGLIT_FAIL);

	/* Check for GL_INVALID_OPERATION when trying to copy framebuffer pixels
	 * to an integer texture when the framebuffer is not an integer format.
	 */
	/* make valid integer texture image here */
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16UI_EXT, 4, 4, 0,
		     GL_RGBA_INTEGER, GL_UNSIGNED_SHORT, NULL);

	glCopyTexSubImage2D(GL_TEXTURE_2D, 0,
			    0, 0, 0, 0, 4, 4);
	if (!piglit_check_gl_error(GL_INVALID_OPERATION))
		piglit_report_result(PIGLIT_FAIL);


	/* can't put non-integer data into an integer texture */
	glTexSubImage2D(GL_TEXTURE_2D, 0,
			0, 0, 4, 4,
			GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	if (!piglit_check_gl_error(GL_INVALID_OPERATION))
		piglit_report_result(PIGLIT_FAIL);


	/* check for invalid format/type combo */
	glTexSubImage2D(GL_TEXTURE_2D, 0,
			0, 0, 4, 4,
			GL_RGBA_INTEGER, GL_FLOAT, NULL);
	if (!piglit_check_gl_error(GL_INVALID_ENUM))
		piglit_report_result(PIGLIT_FAIL);


	/* make valid non-integer texture image here */
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 4, 4, 0,
		     GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	/* This should generate INVALID_OPERATION because the new tex data
	 * is integer but the existing texture image is not.
	 */
	glTexSubImage2D(GL_TEXTURE_2D, 0,
			0, 0, 4, 4,
			GL_RGBA_INTEGER, GL_INT, NULL);
	if (!piglit_check_gl_error(GL_INVALID_OPERATION))
		piglit_report_result(PIGLIT_FAIL);


	/* Creating an integer-valued texture by copying image data from a
	 * non-integer framebuffer should generate GL_INVALID_OPERATION.
	 */
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8UI_EXT, 0, 0, 4, 4, 0);
	if (!piglit_check_gl_error(GL_INVALID_OPERATION))
		piglit_report_result(PIGLIT_FAIL);


	glDeleteTextures(1, &tex);

	return PIGLIT_PASS;
}

void
piglit_init(int argc, char **argv)
{
	piglit_require_extension("GL_EXT_texture_integer");
}
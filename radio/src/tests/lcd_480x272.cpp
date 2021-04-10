/*
 * Copyright (C) OpenTX
 *
 * Based on code named
 *   th9x - http://code.google.com/p/th9x
 *   er9x - http://code.google.com/p/er9x
 *   gruvin9x - http://code.google.com/p/gruvin9x
 *
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <QtCore/QDir>
#include <QtCore/QDebug>
#include <QApplication>
#include <QPainter>
#include <math.h>
#include <gtest/gtest.h>

#define SWAP_DEFINED
#include "opentx.h"
#include "location.h"
#include "targets/simu/simulcd.h"

#if defined(COLORLCD)

#include "gui/colorlcd/fonts.h"

void doPaint_colorlcd(const BitmapBuffer* dc, QPainter & p)
{
  QRgb rgb = qRgb(0, 0, 0);
  p.setBackground(QBrush(rgb));
  p.eraseRect(0, 0, LCD_W, LCD_H);

  uint16_t previousColor = 0xFF;
  for (int y=0; y<LCD_H; y++) {
    for (int x=0; x<LCD_W; x++) {
      auto color = *(dc->getPixelPtr(x, y));  // color in RGB565
      if (color) {
        if (color != previousColor) {
          previousColor = color;
          RGB_SPLIT(color, r, g, b);
          rgb = qRgb(r<<3, g<<2, b<<3);
          p.setPen(rgb);
          p.setBrush(QBrush(rgb));
        }
        p.drawPoint(x, y);
      }
    }
  }
}

bool checkScreenshot_colorlcd(const BitmapBuffer* dc, const QString & test)
{
  QImage buffer(LCD_W, LCD_H, QImage::Format_RGB32);
  QPainter p(&buffer);
  doPaint_colorlcd(dc, p);
  QString filename(QString("%1_%2x%3.png").arg(test).arg(LCD_W).arg(LCD_H));
  QImage reference(TESTS_PATH "/" + filename);

  if (buffer == reference) {
    return true;
  }
  else {
    QString filename(QString("%1_%2x%3.png").arg(test).arg(LCD_W).arg(LCD_H));
    buffer.save("/tmp/" + filename);
    return false;
  }
}


TEST(Lcd_colorlcd, vline)
{
  loadFonts();

  BitmapBuffer dc(BMP_RGB565, LCD_W, LCD_H);
  dc.clear(DEFAULT_BGCOLOR);

  for (int x=0; x<100; x+=2) {
    dc.drawSolidVerticalLine(x, x/2, 12, DEFAULT_COLOR);
  }
  EXPECT_TRUE(checkScreenshot_colorlcd(&dc, "vline"));
}

TEST(Lcd_colorlcd, primitives)
{
  loadFonts();

  BitmapBuffer dc(BMP_RGB565, LCD_W, LCD_H);
  dc.clear(DEFAULT_BGCOLOR);

  dc.drawText(8, 8, "The quick brown fox jumps over the lazy dog", DISABLE_COLOR);
  dc.drawText(5, 5, "The quick brown fox jumps over the lazy dog", DEFAULT_COLOR);

  dc.drawFilledRect(10, 30, 30, 30, SOLID, TITLE_BGCOLOR);
  dc.drawFilledRect(50, 30, 30, 30, DOTTED, DEFAULT_COLOR);

  dc.drawRect(90, 30, 30, 30, 1, SOLID, TITLE_BGCOLOR);
  dc.drawRect(130, 30, 30, 30, 2, SOLID, TITLE_BGCOLOR);
  dc.drawRect(170, 30, 30, 30, 5, SOLID, TITLE_BGCOLOR);

  dc.drawVerticalLine(10, 70, 100, SOLID, TITLE_BGCOLOR);
  dc.drawVerticalLine(15, 70,  90, SOLID, TITLE_BGCOLOR);
  dc.drawVerticalLine(20, 70,  80, SOLID, TITLE_BGCOLOR);
  dc.drawVerticalLine(25, 70,  70, SOLID, TITLE_BGCOLOR);

  dc.drawHorizontalLine(30, 70, 100, SOLID, DEFAULT_COLOR);
  dc.drawHorizontalLine(30, 75,  90, SOLID, DEFAULT_COLOR);
  dc.drawHorizontalLine(30, 80,  80, SOLID, DEFAULT_COLOR);
  dc.drawHorizontalLine(30, 85,  70, SOLID, DEFAULT_COLOR);


  EXPECT_TRUE(checkScreenshot_colorlcd(&dc, "primitives"));
}

TEST(Lcd_colorlcd, transparency)
{
  loadFonts();

  BitmapBuffer dc(BMP_RGB565, LCD_W, LCD_H);
  dc.clear(DEFAULT_BGCOLOR);

  dc.drawText(8, 8, "The quick brown fox jumps over the lazy dog", DEFAULT_COLOR|OPACITY(4));
  dc.drawText(5, 5, "The quick brown fox jumps over the lazy dog", DEFAULT_COLOR|OPACITY(12));

  dc.drawFilledRect(10, 30, 30, 30, SOLID, TITLE_BGCOLOR|OPACITY(8));
  dc.drawFilledRect(50, 30, 30, 30, DOTTED, DEFAULT_COLOR|OPACITY(10));

  dc.drawRect(90, 30, 30, 30, 1, SOLID, TITLE_BGCOLOR|OPACITY(8));
  dc.drawRect(130, 30, 30, 30, 2, SOLID, TITLE_BGCOLOR|OPACITY(8));
  dc.drawRect(170, 30, 30, 30, 5, SOLID, TITLE_BGCOLOR|OPACITY(8));

  dc.drawVerticalLine(10, 70, 100, SOLID, TITLE_BGCOLOR|OPACITY(2));
  dc.drawVerticalLine(15, 70,  90, SOLID, TITLE_BGCOLOR|OPACITY(6));
  dc.drawVerticalLine(20, 70,  80, SOLID, TITLE_BGCOLOR|OPACITY(10));
  dc.drawVerticalLine(25, 70,  70, SOLID, TITLE_BGCOLOR|OPACITY(OPACITY_MAX));

  dc.drawHorizontalLine(30, 70, 100, SOLID, DEFAULT_COLOR|OPACITY(2));
  dc.drawHorizontalLine(30, 75,  90, SOLID, DEFAULT_COLOR|OPACITY(6));
  dc.drawHorizontalLine(30, 80,  80, SOLID, DEFAULT_COLOR|OPACITY(10));
  dc.drawHorizontalLine(30, 85,  70, SOLID, DEFAULT_COLOR|OPACITY(OPACITY_MAX));


  for(int n=0; n<10; n++) {
    int x = 120 + n * 20;
    int y = 80 + n * 10;
    int color = COLOR(n/2 + 5);
    int size = 100;
    dc.drawFilledRect(x, y, size, size, SOLID, color|OPACITY(8));

  }

  EXPECT_TRUE(checkScreenshot_colorlcd(&dc, "transparency"));
}

TEST(Lcd_colorlcd, fonts)
{
  loadFonts();

  BitmapBuffer dc(BMP_RGB565, LCD_W, LCD_H);
  dc.clear(DEFAULT_BGCOLOR);

  dc.drawText(8, 8, "The quick brown fox jumps over the lazy dog", DEFAULT_COLOR|OPACITY(4));
  dc.drawText(5, 5, "The quick brown fox jumps over the lazy dog", DEFAULT_COLOR|OPACITY(12));

  dc.drawText(10, 200, "The quick", DEFAULT_COLOR|VERTICAL);
  dc.drawText(30, 200, "The quick brown fox", DEFAULT_COLOR|VERTICAL);

  dc.drawText(50, 25, "The quick", DEFAULT_COLOR | FONT(XXS));
  dc.drawText(50, 40, "The quick", DEFAULT_COLOR | FONT(XS));
  dc.drawText(50, 55, "The quick", DEFAULT_COLOR | FONT(L));
  dc.drawText(50, 80, "The quick", DEFAULT_COLOR | FONT(XL));
  dc.drawText(50, 120, "The quick", DEFAULT_COLOR | FONT(XXL));

  dc.drawText(8, 208, "The quick brown fox jumps over the lazy dog", TITLE_BGCOLOR|OPACITY(4));
  dc.drawText(5, 205, "The quick brown fox jumps over the lazy dog", TITLE_BGCOLOR|OPACITY(12));

  EXPECT_TRUE(checkScreenshot_colorlcd(&dc, "fonts"));
}

TEST(Lcd_colorlcd, clipping)
{
  loadFonts();

  BitmapBuffer dc(BMP_RGB565, LCD_W, LCD_H);
  dc.clear(DEFAULT_BGCOLOR);

  dc.drawSolidVerticalLine(100, 0, LCD_H, DEFAULT_COLOR);
  dc.drawSolidVerticalLine(400, 0, LCD_H, DEFAULT_COLOR);

  dc.drawSolidHorizontalLine(0,  50, LCD_W, DEFAULT_COLOR);
  dc.drawSolidHorizontalLine(0, 200, LCD_W, DEFAULT_COLOR);

  dc.setClippingRect(100, 400, 50, 200);

  dc.drawSolidHorizontalLine(0, 80, LCD_W, TITLE_BGCOLOR);
  dc.drawHorizontalLine(     0, 81, LCD_W, SOLID, TITLE_BGCOLOR);

  dc.drawSolidVerticalLine(150,  0, LCD_H, TITLE_BGCOLOR);
  dc.drawVerticalLine(     151,  0, LCD_H, SOLID, TITLE_BGCOLOR);

  dc.drawSolidRect(20, 20, 50, 50, 2, TITLE_BGCOLOR);
  dc.drawRect(    380, 20, 50, 50, 2, SOLID, TITLE_BGCOLOR);
  
  EXPECT_TRUE(checkScreenshot_colorlcd(&dc, "clipping"));
}

#endif

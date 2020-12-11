/*
 * Copyright (C) 2019 lombo Tech CO.,LTD
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.lombo.app.view;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.PorterDuff;
import android.graphics.RectF;
import android.util.AttributeSet;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import com.lombo.app.faceid.Person;
import com.lombo.app.faceid.api;
import com.lombo.app.utils.UsbConnectController;

import java.util.List;


public class RectView extends SurfaceView implements SurfaceHolder.Callback {

  private final Object mSync = new Object();

  private DrawRectThread mDrawRectThread;

  /** switch drawing rect */
  private boolean mDrawingRect = true;

  /** switch drawing angle */
  private boolean mDrawingAngle = false;

  /** check living */
  private boolean mCheckLiving = false;

  private DataSrcType mDataSrcType = DataSrcType.USB_CONTROLLER;

  public void setDrawingRect(boolean drawingRect) {
    this.mDrawingRect = drawingRect;
  }

  public void setDrawingAngle(boolean drawingAngle) {
    this.mDrawingAngle = drawingAngle;
  }

  public void setCheckLiving(Boolean checkLiving) {
    // Log.i("RectView", "setCheckLiving: mCheckLiving: " + mCheckLiving);
    this.mCheckLiving = checkLiving;
  }

  private Paint mGreenRectPaint = new Paint();
  private Paint mGreenTextPaint = new Paint();
  private Paint mRedRectPaint = new Paint();
  private Paint mRedTextPaint = new Paint();
  private Paint mWhitePaint = new Paint();

  private int mWidth = 0;
  private int mHeight = 0;

  public RectView(Context context, AttributeSet attrs) {
    super(context, attrs);
    DisplayMetrics displayMetrics = getContext().getResources().getDisplayMetrics();

    mWidth = displayMetrics.widthPixels;
    mHeight = displayMetrics.heightPixels;
    Log.i("RectView", "RectView: width: " + mWidth + ", height: " + mHeight);

    mGreenRectPaint.setStyle(Paint.Style.STROKE);
    mGreenRectPaint.setStrokeWidth(5f);
    mGreenRectPaint.setColor(Color.GREEN);

    mGreenTextPaint.setStyle(Paint.Style.FILL);
    mGreenTextPaint.setStrokeWidth(5f);
    mGreenTextPaint.setColor(Color.GREEN);
    mGreenTextPaint.setTextAlign(Paint.Align.CENTER);
    mGreenTextPaint.setTextSize(30);

    mRedRectPaint.setStyle(Paint.Style.STROKE);
    mRedRectPaint.setStrokeWidth(5f);
    mRedRectPaint.setColor(Color.RED);

    mRedTextPaint.setStyle(Paint.Style.FILL);
    mRedTextPaint.setStrokeWidth(5f);
    mRedTextPaint.setColor(Color.RED);
    mRedTextPaint.setTextAlign(Paint.Align.CENTER);
    mRedTextPaint.setTextSize(40);

    mWhitePaint.setStyle(Paint.Style.FILL);
    mWhitePaint.setStrokeWidth(5f);
    mWhitePaint.setTextAlign(Paint.Align.CENTER);
    mWhitePaint.setColor(Color.WHITE);
    mWhitePaint.setTextSize(40);
    getHolder().addCallback(this);
  }

  public void onResume() {
    synchronized (mSync) {
      if (mDrawRectThread == null) {
        mDrawRectThread = new DrawRectThread();
        mDrawRectThread.start();
      }
    }
  }

  public void onPause() {
    synchronized (mSync) {
      if (mDrawRectThread != null) {
        mDrawRectThread.onStop();
        mDrawRectThread = null;
      }
    }
    Canvas canvas = getHolder().lockCanvas();
    if (canvas == null) {
      return;
    }
    canvas.drawColor(Color.TRANSPARENT, PorterDuff.Mode.CLEAR);
    getHolder().unlockCanvasAndPost(canvas);
  }

  public enum DataSrcType{API, USB_CONTROLLER}
  public void setDataSrcType(DataSrcType dataSrcType) {
    mDataSrcType = dataSrcType;
  }


  @Override
  public void surfaceCreated(SurfaceHolder holder) {
//     Log.i("surfaceview", "surfaceCreated: ");
//    onResume();
  }

  @Override
  public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {

  }

  @Override
  public void surfaceDestroyed(SurfaceHolder holder) {
    onPause();
  }

  private class DrawRectThread extends Thread {
    private boolean mDoing = true;
    public void onStop() {
      mDoing = false;
    }

    @Override
    public void run() {
      while (mDoing) {
        int faceNum = 0;
        Canvas canvas = RectView.this.getHolder().lockCanvas();
        try {
          canvas.drawColor(Color.TRANSPARENT, PorterDuff.Mode.CLEAR);
          List<Person> list = null;
          if (RectView.this.mDataSrcType == DataSrcType.USB_CONTROLLER) {
            list = UsbConnectController.getInstance().getPersonList();
          } else if (RectView.this.mDataSrcType == DataSrcType.API) {
            list = api.getInstance().getPersonList();
          }

          for(Person person : list) {
            if (person.getLiving() != 1) {
              if (RectView.this.mCheckLiving) {
                continue;
              }
            }
            faceNum++;
            if (mDrawingRect) {
              String msg = "";
              RectF rect = new RectF(((float) person.getRect()[0] / 1280) * mWidth, ((float) person.getRect()[1] / 720) * mHeight,
                      ((float) person.getRect()[2] / 1280) * mWidth, ((float) person.getRect()[3] / 720) * mHeight);

              if (person.getRecg() == 1) {
                if (!person.getName().equals("")) {
                  msg = person.getName();
                  if (mDrawingAngle) {
                    msg += " [ ";
                    msg += "Y:" + person.getHead_pose()[0].substring(0, person.getHead_pose()[0].indexOf("."));
                    msg += " P:" + person.getHead_pose()[1].substring(0, person.getHead_pose()[1].indexOf("."));
                    msg += " R:" + person.getHead_pose()[2].substring(0, person.getHead_pose()[2].indexOf("."));
                    msg += " ]";
                  }
                }
              }
              // msg += (person.getMale()==1?"男" : "女");

              Paint rectPaint = null;
              Paint textPaint = null;
              if (RectView.this.mCheckLiving) {
                rectPaint = mGreenRectPaint;
                textPaint = mGreenTextPaint;
              } else {
                rectPaint = mRedRectPaint;
                textPaint = mRedTextPaint;
              }
              canvas.drawRoundRect(rect, 15f, 15f, rectPaint);

              int textHeight = person.getRect()[1] - 10;
              if (person.getRect()[1] < 50) {
                textHeight = person.getRect()[3] + 35;
              }
              canvas.drawText(msg, ((float)(person.getRect()[0] + person.getRect()[2])/2/1280) * mWidth,
                      ((float) textHeight / 720) * mHeight, textPaint);
            }
          }
        } catch (Exception e) {
//          e.printStackTrace();
        } finally {
          try {
            canvas.drawText("当前人脸数：" + faceNum, getWidth() >> 1, 100, mWhitePaint);
            RectView.this.getHolder().unlockCanvasAndPost(canvas);
            Thread.sleep(50);
          } catch (InterruptedException e) {
//            e.printStackTrace();
          } catch (Exception e) {
//            e.printStackTrace();
          }
        }
      }
    }
  }
}

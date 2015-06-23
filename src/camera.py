#!/usr/bin/python
# -*- coding: utf-8 -*-

import cv
from datetime import datetime

if __name__ == '__main__':
    capture = cv.CaptureFromCAM(0)
    cv.NamedWindow('Webcam')
    
    while True:
        frame = cv.QueryFrame(capture)
        cv.ShowImage('Webcam', frame)
        
        c = cv.WaitKey(10) % 256
        
        if c == 27:
            # ESC pressed. Finish the program
            break
        elif c == 10:
            # ENTER pressed. Store image to disk
            cv.SaveImage("foto" + '.bmp', frame)
    
    capture = None
    cv.DestroyAllWindows()


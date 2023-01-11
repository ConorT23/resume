import cv2
import os

def takeImage(vid, src, view):

    if src == 'Secondary':
        cam = cv2.VideoCapture(1) #1 is secondary cam
    else :
        cam = cv2.VideoCapture(0) #0 is computer defualt cam
    

    # cv2.namedWindow("FIZ App")

    while True:
        ret, frame = cam.read()
        if not ret:
            print('failed')
            break
        cv2.imshow('Take Image', frame)

        k = cv2.waitKey(1)
        if k%256 == 27:
            # ESC pressed
            break
        elif k%256 == 32:
            # SPACE pressed

            # imgName = str(vid)+'.png'
            # cv2.imwrite(imgName, frame)
            if view == "tool":
                path = str(os.getcwd())+'/itemsImg'
            elif view == "rental":
                path = str(os.getcwd())+'/rentalsImg'
                
            cv2.imwrite(os.path.join(path , str(vid)+'.png'),frame)

            break

        if cv2.getWindowProperty('Take Image',cv2.WND_PROP_VISIBLE) < 1:        
            break 

    cam.release()
    cv2.destroyAllWindows()

(in-package :sa)


(defun aztec-allocator ()
  (or (nsys:find-allocator :aztec nil)
      (nsys:make-allocator :aztec :memory)))

(defun dump-raster (raster name &key alpha-code bitmap-p (out-dir "rstr-2"))
  (let* ((c-name (substitute #\_ #\- name))
         
         (hf-name (concatenate 'string
                    "/home/dm/coco/" out-dir "/" name ".h"))
         (pixel (color-i::make-pixel raster))
         (ar (and alpha-code (logand #xff alpha-code)))
         (ag (and alpha-code (logand #xff (ash alpha-code -8))))
         (ab (and alpha-code (logand #xff (ash alpha-code -16))))
         (aa (and alpha-code (ash alpha-code -24)))
         (ww-name (format nil "COCO_~A_RASTER_W"  (string-upcase c-name)))
         (hh-name (format nil "COCO_~A_RASTER_H"  (string-upcase c-name)))
         ;; width in pixels, not words or bytes
         (width (if bitmap-p 
                    (* 32 (ceiling (color:raster-width raster) 32))
                    (color:raster-width raster)))
         (height    (color:raster-height raster))
         )
    (declare (ignore aa))
    ;;(excl:run-shell-command "touch /net/home/dm/space-aztec/c/rstr/timestamp")
    (color-i::with-c-raster (c-raster raster)
      (with-open-file (s hf-name
                       :direction :output 
                       :if-does-not-exist :create 
                       :if-exists :supersede)
        (format s "#define ~A ~A~%#define ~A ~A~%~%extern struct coco_raster coco_~A_raster;~%"
                ww-name width hh-name height c-name))
      (with-open-file (s (concatenate
                             'string "/home/dm/coco/" out-dir "/" name ".c")
                       :direction :output 
                       :if-does-not-exist :create 
                       :if-exists :supersede)
        (format s "#include \"../raster.h\"~%#include \"~A\"~%~%" 
                (concatenate 'string name ".h"))
        ;;(format s "#ifdef COCO_RASTER_DATA~%")
        (format s "static unsigned char coco_pixels[~A] = {~%"
                (if bitmap-p
                    (format nil "~A*~A>>3" ww-name hh-name)
                    (format nil "~A*~A*4" ww-name hh-name)))
        #+ignore(if bitmap-p (write-char #\newline))
        (loop for y below height
            as yw from 0 by width do
              (format s "  ")
              (if bitmap-p
                  (let ((bit-idx   7)
                        (byte-idx 0)
                        (pixel-acc 0))
                    (loop for x below (color:raster-width raster);; NOT: width 
                        as side-effect-garbage = 
                          (color-i::%raster-read-pixel-integer 
                           c-raster x y pixel)

                        as number = (aref pixel 0)
                        as r = (logand #xFF number)
                        as g = (logand #xFF (ash number -8))
                        as b = (logand #xFF (ash number -16))
                        ;;as a = (ash number -24)
                        as pixel-val 
                        = (if (< 128 (* (+ r g b) 0.333333)) 1 0)
                        do
                          side-effect-garbage
                          ;;(format t "pixel-acc = ~8B~%" pixel-acc)
                          (setf pixel-acc 
                            (logior pixel-acc (ash pixel-val bit-idx))
                            bit-idx (1- bit-idx))
                          (when (minusp bit-idx)
                            #+ignore
                            (dotimes (i 8)
                              (write-char 
                               (if (plusp (logand (ash 1 (- 7 i)) pixel-acc))
                                   #\X
                                   #\ )))
                            (format s "~A," pixel-acc)
                            (setf bit-idx 7
                                  byte-idx (mod (1+ byte-idx) 4)
                                  pixel-acc 0))
                        finally
                          (when (/= bit-idx 7)
                            (format s "   ~A," pixel-acc)
                            (setf byte-idx (mod (1+ byte-idx) 4)))
                          ;; word align rasters.
                          (dotimes (i (mod (- 4 byte-idx) 4))
                            (format s "0,")))
                    #+ignore(write-char #\newline))
                  (loop for x below width do
                        (color-i::%raster-read-pixel-integer 
                         c-raster x y pixel)
                        (let* ((number (aref pixel 0))
                               (r (logand #xFF number))
                               (g (logand #xFF (ash number -8)))
                               (b (logand #xFF (ash number -16)))
                               (a (ash number -24)))
                          (if (and alpha-code (= r ar) (= g ag) (= b ab))
                              (format s "~A,~A,~A,~A, " 0 0 0 0)
                              (if alpha-code
                                  (format s "~A,~A,~A,~A, " r g b 255)
                                  (format s "~A,~A,~A,~A, " r g b a))))))
              (terpri s))
        (format s "};~%")
        ;;(format s "#endif /* COCO_RASTER_DATA */~%")
        (format s "struct coco_raster coco_~A_raster = {~%" c-name)
        (format s "  COCO_~A_RASTER_W,~%  COCO_~A_RASTER_H,~%  ~A, 0,~%coco_pixels};~%"
                ;; "  COCO_~A_RASTER_W,~%  COCO_~A_RASTER_H,~%  ~A, 0,~%#ifdef COCO_RASTER_DATA~%  coco_pixels~%#else~%  0~%#endif~%};~%"
                (string-upcase c-name) (string-upcase c-name)
                (if bitmap-p "COCO_BITMAP"
                    (if alpha-code "COCO_RGBA_8" "COCO_RGBA_8_BLEND")))))))


(defparameter *red-coded-files*
    '("x" "forward" "back" "question" "check"))

(defun dump-directory-rasters (&key (directory #p"~dm/space-aztec-old/tiffs/")
                                    (alpha-code 0) (bitmap-p)
                                    (out-dir (error "Specify :out-dir")))
  (let ((tiffs nil))
    (labels 
        ((process-dir (filename bitmap-p)
           (loop for f in (directory filename :directories-are-files nil)
               when (or (string= (pathname-type f) "tiff")
                        (string= (pathname-type f) "tif"))
               do 
                 (push (pathname-name f) tiffs)
                 (let ((r (xtim:load-image-raster f (aztec-allocator))))
                   (if r
                       (progn
                         (color:flip-raster r :vertical t)
                         (dump-raster r (pathname-name f) 
                                      :alpha-code 
                                      (if (or bitmap-p
                                              (string= (pathname-type f) "tif"))
                                          nil
                                          (if (member (pathname-name f)
                                                      *red-coded-files* 
                                                      :test 'string=)
                                              #xFF0000FF
                                              alpha-code))
                                      :bitmap-p bitmap-p
                                      :out-dir out-dir))
                       (warn "Problems loading ~A" f)))
               when (and (not bitmap-p)
                         (string= (car (last (pathname-directory f))) "bitmaps"))
               do (process-dir f t))))
      (process-dir directory bitmap-p)
      (setf tiffs (nreverse tiffs))
      (with-open-file (s (concatenate 'string "/home/dm/coco/" out-dir "/rasters.h")
                       :direction :output 
                       :if-does-not-exist :create 
                       :if-exists :new-version)
        (format s "#ifndef COCO_RASTERS_H~%#define COCO_RASTERS_H~%~%")
        (loop for f in tiffs do
              (format s "#include \"~A.h\"~%" f))
        (format s "~%#endif~%")))))


(defun resuffixize-directory (old-suffix new-suffix 
                              directory) ;  #p"~dm/space-aztec/tiffs/"
                              
  (labels 
      ((process-dir (filename)
         (loop for f in (directory filename :directories-are-files nil)
             when (string= (pathname-type f) old-suffix)
             do 
               (rename-file f (make-pathname :defaults f :type new-suffix)))))
    (process-dir directory)))

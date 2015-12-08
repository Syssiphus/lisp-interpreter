
(define *size* '(50.0 . 50.0))
(define *height* (cdr *size*))
(define *width* (car *size*))

(define *w* 5.0)
(define *h* (quotient (* *w* *height*) *width*))

(define *xmin* (quotient (- *w*) 2))
(define *ymin* (quotient (- *h*) 2))

(define *maxiterations* 100.0)

(define *xmax* (+ *xmin* *w*))
(define *ymax* (+ *ymin* *h*))

(define *dx* (quotient (- *xmax* *xmin*) *width*))
(define *dy* (quotient (- *ymax* *ymin*) *height*))

(define *symbols* '(#\# #\0 #\\ #\o #\: #\_ #\. #\space))

(define (mandelbrot)
  (y-iter *ymin* 0 *height* '()))
  ;;(print-mandelbrot (y-iter *ymin* 0 *height* '())))

(define (print-mandelbrot m)
  (print-mandelbrot-r m 0))

(define (print-mandelbrot-r m y)
  (if (= y *height*)
    #t
    (begin
      (print-mandelbrot-line m 0)
      (print-mandelbrot-r (list-tail m *width*) (+ y 1)))))

(define (print-mandelbrot-line m x)
  (if (= x *width*)
    (write-char #\newline)
    (begin
      (write-char (get-symbol (car m)))
      (print-mandelbrot-line (cdr m) (+ x 1)))))

(define (get-symbol i)
  (let ((percent (quotient i *maxiterations*)))
   (let ((position (floor (* (length *symbols*) percent))))
    (if (< position (length *symbols*))
      (list-ref *symbols* position)
      (error "Illegal position.")))))

(define (y-iter y count end acc)
  (if (= count end)
    (reverse acc)
    (y-iter (+ 1 y) (+ 1 count) end 
            (cons (x-iter *xmin* y 0 *width* '()) acc))))

(define (x-iter x y count end acc)
  (if (= count end)
    acc
    (begin
      (set! acc (iter x y x y 0 *maxiterations* acc))
      (x-iter (+ 1 x) y (+ 1 count) end acc))))

(define (iter x y a b count maxiterations acc)
  (if (= count maxiterations)
    acc
    (let ((aa (* a a))
          (bb (* b b))
          (twoab (* 2.0 a b)))
      (if (> (+ aa bb) 16.0)
        (cons count acc)
        (iter x y (+ (- aa bb) x) (+ twoab y) (+ 1 count) maxiterations acc)))))
      


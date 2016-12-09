;; Functions from the R5RS standard

(define (caar x) (car (car x)))
(define (cadr x) (car (cdr x)))
(define (cdar x) (cdr (car x)))
(define (cddr x) (cdr (cdr x)))
(define (caaar x) (car (car (car x))))
(define (caadr x) (car (car (cdr x))))
(define (cadar x) (car (cdr (car x))))
(define (caddr x) (car (cdr (cdr x))))
(define (cdaar x) (cdr (car (car x))))
(define (cdadr x) (cdr (car (cdr x))))
(define (cddar x) (cdr (cdr (car x))))
(define (cdddr x) (cdr (cdr (cdr x))))
(define (caaaar x) (car (car (car (car x)))))
(define (caaadr x) (car (car (car (cdr x)))))
(define (caadar x) (car (car (cdr (car x)))))
(define (caaddr x) (car (car (cdr (cdr x)))))
(define (cadaar x) (car (cdr (car (car x)))))
(define (cadadr x) (car (cdr (car (cdr x)))))
(define (caddar x) (car (cdr (cdr (car x)))))
(define (cadddr x) (car (cdr (cdr (cdr x)))))
(define (cdaaar x) (cdr (car (car (car x)))))
(define (cdaadr x) (cdr (car (car (cdr x)))))
(define (cdadar x) (cdr (car (cdr (car x)))))
(define (cdaddr x) (cdr (car (cdr (cdr x)))))
(define (cddaar x) (cdr (cdr (car (car x)))))
(define (cddadr x) (cdr (cdr (car (cdr x)))))
(define (cdddar x) (cdr (cdr (cdr (car x)))))
(define (cddddr x) (cdr (cdr (cdr (cdr x)))))

(define (null? exp)
  (if (eqv? exp '())
      #t #f))

(define (length items)
  (define (iter a count)
    (if (null? a)
        count
        (iter (cdr a) (+ 1 count))))
  (iter items 0))

(define (append list1 list2)
  (if (null? list1)
      list2
      (cons (car list1) (append (cdr list1) list2))))

(define (reverse l)
  (define (iter in out)
    (if (pair? in)
        (iter (cdr in) (cons (car in) out))
        out))
  (iter l '()))

(define (remove x l)
  (define (iter i acc)
    (cond ((null? i) (reverse acc))
          ((eq? x (car i)) (iter (cdr i) acc))
          (else (iter (cdr i) (cons (car i) acc)))))
  (iter l '()))

(define (map proc items)
  (if (null? items)
      '()
      (cons (proc (car items))
            (map proc (cdr items)))))

(define (for-each f l)
  (if (null? l)
      #t
      (begin
        (f (car l))
        (for-each f (cdr l)))))

(define (not x)
  (if (and (boolean? x) 
           (eqv? x #f))
      #t
      #f))

(define (zero? x)
  (if (and (number? x) 
           (= x 0))
      #t
      #f))

(define (positive? x)
  (if (and (number? x) 
           (> x 0))
      #t
      #f))

(define (negative? x)
  (if (and (number? x) 
           (< x 0))
      #t
      #f))

(define (odd? x)
  (if (and (integer? x)
           (= (remainder x 2) 1))
      #t
      #f))

(define (even? x)
  (if (and (integer? x)
           (= (remainder x 2) 0))
      #t
      #f))

(define (max x y)
  (if (> x y)
      x
      y))

(define (min x y)
  (if (< x y)
      x
      y))

(define (abs x)
  (if (negative? x)
      (- x)
      x))

(define (list . args) args)

(define (list-tail l k)
  (if (zero? k)
      l
      (list-tail (cdr l) (- k 1))))

(define (list-ref l k)
  (if (zero? k)
      (car l)
      (list-ref (cdr l) (- k 1))))

(define (member l k)
  (cond ((null? l) #f)
        ((eq? (car l) k) #t))
  (member (cdr l) k))

;; Display of information
(define (newline)
  (display #\newline))

(define (display str)
  (let ((output-port current-output-port))

    (define (str-iter str pos len)
      (if (zero? (- pos len))
          #t
          (begin
            (write-char (string-ref str pos) output-port)
            (str-iter str (+ pos 1) len))))

    (cond ((char? str) (write-char str) output-port)
          ((string? str) (str-iter str 0 (string-length str)))
          ((number? str) (let ((output-str (number->string str)))
                           (str-iter output-str 0 (string-length output-str))))
          (else #f))
    #t))

;; assoc lists
(define (assoc obj alist)
  (cond ((eq? alist '()) #f)
        ((eq? obj (car (car alist))) (car alist))
        (else (assoc obj (cdr alist)))))

;; vectors
(define (vector . args)
  (list->vector args))

(define (vector->list v)
  (define (vec-to-list vec len acc)
    (if (zero? len)
        acc
        (vec-to-list v (- len 1) (cons (vector-ref v (- len 1)) acc))))
  (vec-to-list v (vector-length v) '()))

(define (list->vector l)
  (define (set-values vec l pos len)
    (if (>= pos len)
        vec
        (begin
          (vector-set! vec pos (car l))
          (set-values vec (cdr l) (+ pos 1) len))))
  (let ((len (length l)))
    (let ((vec (make-vector len)))
      (set-values vec l 0 len))))

(define (vector-fill! vec obj)
  (let ((len (vector-length vec)))
    (do ((i 0 (+ i 1)))
        ((>= i len))
      (vector-set! vec i obj))))

;; sleep
(define (msleep x)
  (sleep (/ x 1000)))

(define (usleep x)
  (sleep (/ x 1000000)))

;; Some additional functions
(define (assert x y)
  (if (eqv? x y)
      #t
      (error "assert failed" x "!=" y)))

(define (assert-true x)
  (if x
      #t
      (error "assert failed.")))

(define (assert-false x)
  (if x
      (error "assert failed.")
      #t))



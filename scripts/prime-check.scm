
(define (is-prime? n)
  (define (is-prime-iter i)
    (if (> (* i i) n)
        #t
        (if (or (zero? (modulo n i)) (zero? (modulo n (+ i 2))))
            #f
            (is-prime-iter (+ i 6)))))
  (cond ((<= n 1) #f)
        ((<= n 3) #t)
        ((or (zero? (modulo n 2)) (zero? (modulo n 3))) #f)
        (else (is-prime-iter 5))))

(define (make-account balance)
  (define (withdraw amount)
    (if (>= balance amount)
        (begin (set! balance (- balance amount))
               balance)
        "Insufficient funds."))
  (define (deposit amount)
    (set! balance (+ balance amount))
    balance)
  (define (dispatch m)
    (cond ((eqv? m 'withdraw) withdraw)
          ((eqv? m 'deposit) deposit)
          (else (error "Unknown request -- MAKE ACCOUNT"
                       m))))
  dispatch)


